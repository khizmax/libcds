// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_ELLEN_BINTREE_RCU_H
#define CDSLIB_INTRUSIVE_ELLEN_BINTREE_RCU_H

#include <memory>
#include <cds/intrusive/details/ellen_bintree_base.h>
#include <cds/opt/compare.h>
#include <cds/details/binary_functor_wrapper.h>
#include <cds/urcu/details/check_deadlock.h>
#include <cds/urcu/exempt_ptr.h>

namespace cds { namespace intrusive {
    //@cond
    namespace ellen_bintree {

        template <class RCU>
        struct base_node<cds::urcu::gc<RCU> >: public basic_node
        {
            typedef basic_node base_class;

            base_node * m_pNextRetired;

            typedef cds::urcu::gc<RCU> gc       ;   ///< Garbage collector

            /// Constructs leaf (bIntrenal == false) or internal (bInternal == true) node
            explicit base_node( bool bInternal )
                : basic_node( bInternal )
                , m_pNextRetired( nullptr )
            {}
        };

    } // namespace ellen_bintree
    //@endcond

    /// Ellen's et al binary search tree (RCU specialization)
    /** @ingroup cds_intrusive_map
        @ingroup cds_intrusive_tree
        @anchor cds_intrusive_EllenBinTree_rcu

        Source:
            - [2010] F.Ellen, P.Fatourou, E.Ruppert, F.van Breugel "Non-blocking Binary Search Tree"

        %EllenBinTree is an unbalanced leaf-oriented binary search tree that implements the <i>set</i>
        abstract data type. Nodes maintains child pointers but not parent pointers.
        Every internal node has exactly two children, and all data of type \p T currently in
        the tree are stored in the leaves. Internal nodes of the tree are used to direct \p find
        operation along the path to the correct leaf. The keys (of \p Key type) stored in internal nodes
        may or may not be in the set. \p Key type is a subset of \p T type.
        There should be exactly defined a key extracting functor for converting object of type \p T to
        object of type \p Key.

        Due to \p extract_min and \p extract_max member functions the \p %EllenBinTree can act as
        a <i>priority queue</i>. In this case you should provide unique compound key, for example,
        the priority value plus some uniformly distributed random value.

        @attention Recall the tree is <b>unbalanced</b>. The complexity of operations is <tt>O(log N)</tt>
        for uniformly distributed random keys, but in the worst case the complexity is <tt>O(N)</tt>.

        @note In the current implementation we do not use helping technique described in the original paper.
        Instead of helping, when a thread encounters a concurrent operation it just spins waiting for
        the operation done. Such solution allows greatly simplify the implementation of tree.

        <b>Template arguments</b>:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p Key - key type, a subset of \p T
        - \p T - type to be stored in tree's leaf nodes. The type must be based on \p ellen_bintree::node
            (for \p ellen_bintree::base_hook) or it must have a member of type \p ellen_bintree::node
            (for \p ellen_bintree::member_hook).
        - \p Traits - tree traits, default is \p ellen_bintree::traits
            It is possible to declare option-based tree with \p ellen_bintree::make_traits metafunction
            instead of \p Traits template argument.

        @note Before including <tt><cds/intrusive/ellen_bintree_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.

        @anchor cds_intrusive_EllenBinTree_rcu_less
        <b>Predicate requirements</b>

        \p Traits::less, \p Traits::compare and other predicates using with member fuctions should accept at least parameters
        of type \p T and \p Key in any combination.
        For example, for \p Foo struct with \p std::string key field the appropiate \p less functor is:
        \code
        struct Foo: public cds::intrusive::ellen_bintree::node< ... >
        {
            std::string m_strKey;
            ...
        };

        struct less {
            bool operator()( Foo const& v1, Foo const& v2 ) const
            { return v1.m_strKey < v2.m_strKey ; }

            bool operator()( Foo const& v, std::string const& s ) const
            { return v.m_strKey < s ; }

            bool operator()( std::string const& s, Foo const& v ) const
            { return s < v.m_strKey ; }

            // Support comparing std::string and char const *
            bool operator()( std::string const& s, char const * p ) const
            { return s.compare(p) < 0 ; }

            bool operator()( Foo const& v, char const * p ) const
            { return v.m_strKey.compare(p) < 0 ; }

            bool operator()( char const * p, std::string const& s ) const
            { return s.compare(p) > 0; }

            bool operator()( char const * p, Foo const& v ) const
            { return v.m_strKey.compare(p) > 0; }
        };
        \endcode

        @anchor cds_intrusive_EllenBinTree_usage
        <b>Usage</b>

        Suppose we have the following Foo struct with string key type:
        \code
        struct Foo {
            std::string     m_strKey    ;   // The key
            //...                           // other non-key data
        };
        \endcode

        We want to utilize RCU-based \p %cds::intrusive::EllenBinTree set for \p Foo data.
        We may use base hook or member hook. Consider base hook variant.
        First, we need deriving \p Foo struct from \p cds::intrusive::ellen_bintree::node:
        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/intrusive/ellen_bintree_rcu.h>

        // RCU type we use
        typedef cds::urcu::gc< cds::urcu::general_buffered<> >  gpb_rcu;

        struct Foo: public cds::intrusive:ellen_bintree::node< gpb_rcu >
        {
            std::string     m_strKey    ;   // The key
            //...                           // other non-key data
        };
        \endcode

        Second, we need to implement auxiliary structures and functors:
        - key extractor functor for extracting the key from \p Foo object.
            Such functor is necessary because the tree internal nodes store the keys.
        - \p less predicate. We want our set should accept \p std::string
            and <tt>char const *</tt> parameters for searching, so our \p less
            predicate will not be trivial, see below.
        - item counting feature: we want our set's \p size() member function
            returns actual item count.

        \code
        // Key extractor functor
        struct my_key_extractor
        {
            void operator ()( std::string& key, Foo const& src ) const
            {
                key = src.m_strKey;
            }
        };

        // Less predicate
        struct my_less {
            bool operator()( Foo const& v1, Foo const& v2 ) const
            { return v1.m_strKey < v2.m_strKey ; }

            bool operator()( Foo const& v, std::string const& s ) const
            { return v.m_strKey < s ; }

            bool operator()( std::string const& s, Foo const& v ) const
            { return s < v.m_strKey ; }

            // Support comparing std::string and char const *
            bool operator()( std::string const& s, char const * p ) const
            { return s.compare(p) < 0 ; }

            bool operator()( Foo const& v, char const * p ) const
            { return v.m_strKey.compare(p) < 0 ; }

            bool operator()( char const * p, std::string const& s ) const
            { return s.compare(p) > 0; }

            bool operator()( char const * p, Foo const& v ) const
            { return v.m_strKey.compare(p) > 0; }
        };

        // Tree traits for our set
        // It is necessary to specify only those typedefs that differ from
        // cds::intrusive::ellen_bintree::traits defaults.
        struct set_traits: public cds::intrusive::ellen_bintree::traits
        {
            typedef cds::intrusive::ellen_bintree::base_hook< cds::opt::gc<gpb_rcu> > > hook;
            typedef my_key_extractor    key_extractor;
            typedef my_less             less;
            typedef cds::atomicity::item_counter item_counter;
        };
        \endcode

        Now we declare \p %EllenBinTree set and use it:
        \code
        typedef cds::intrusive::EllenBinTree< gpb_rcu, std::string, Foo, set_traits >   set_type;

        set_type    theSet;
        // ...
        \endcode

        Instead of declaring \p set_traits type traits we can use option-based syntax with
        \p ellen_bintree::make_traits metafunction, for example:
        \code
        typedef cds::intrusive::EllenBinTree< gpb_rcu, std::string, Foo,
            typename cds::intrusive::ellen_bintree::make_traits<
                cds::opt::hook< cds::intrusive::ellen_bintree::base_hook< cds::opt::gc<gpb_rcu> > >
                ,cds::intrusive::ellen_bintree::key_extractor< my_key_extractor >
                ,cds::opt::less< my_less >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type2;
        \endcode

        Functionally, \p set_type and \p set_type2 are equivalent.

        <b>Member-hooked tree</b>

        Sometimes, we cannot use base hook, for example, when the \p Foo structure is external.
        In such case we can use member hook feature.
        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/intrusive/ellen_bintree_rcu.h>

        // Struct Foo is external and its declaration cannot be modified.
        struct Foo {
            std::string     m_strKey    ;   // The key
            //...                           // other non-key data
        };

        // RCU type we use
        typedef cds::urcu::gc< cds::urcu::general_buffered<> >  gpb_rcu;

        // Foo wrapper
        struct MyFoo
        {
            Foo     m_foo;
            cds::intrusive:ellen_bintree::node< gpb_rcu >   set_hook;   // member hook
        };

        // Key extractor functor
        struct member_key_extractor
        {
            void operator ()( std::string& key, MyFoo const& src ) const
            {
                key = src.m_foo.m_strKey;
            }
        };

        // Less predicate
        struct member_less {
            bool operator()( MyFoo const& v1, MyFoo const& v2 ) const
            { return v1.m_foo.m_strKey < v2.m_foo.m_strKey ; }

            bool operator()( MyFoo const& v, std::string const& s ) const
            { return v.m_foo.m_strKey < s ; }

            bool operator()( std::string const& s, MyFoo const& v ) const
            { return s < v.m_foo.m_strKey ; }

            // Support comparing std::string and char const *
            bool operator()( std::string const& s, char const * p ) const
            { return s.compare(p) < 0 ; }

            bool operator()( MyFoo const& v, char const * p ) const
            { return v.m_foo.m_strKey.compare(p) < 0 ; }

            bool operator()( char const * p, std::string const& s ) const
            { return s.compare(p) > 0; }

            bool operator()( char const * p, MyFoo const& v ) const
            { return v.m_foo.m_strKey.compare(p) > 0; }
        };

        // Tree traits for our member-based set
        struct member_set_traits: public cds::intrusive::ellen_bintree::traits
        {
            cds::intrusive::ellen_bintree::member_hook< offsetof(MyFoo, set_hook), cds::opt::gc<gpb_rcu> > > hook;
            typedef member_key_extractor    key_extractor;
            typedef member_less             less;
            typedef cds::atomicity::item_counter item_counter;
        };

        // Tree containing MyFoo objects
        typedef cds::intrusive::EllenBinTree< gpb_rcu, std::string, MyFoo, member_set_traits >   member_set_type;

        member_set_type    theMemberSet;
        \endcode

        <b>Multiple containers</b>

        Sometimes we need that our \p Foo struct should be used in several different containers.
        Suppose, \p Foo struct has two key fields:
        \code
        struct Foo {
            std::string m_strKey    ;   // string key
            int         m_nKey      ;   // int key
            //...                       // other non-key data fields
        };
        \endcode

        We want to build two intrusive \p %EllenBinTree sets: one indexed on \p Foo::m_strKey field,
        another indexed on \p Foo::m_nKey field. To decide such case we should use a tag option for
        tree's hook:
        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/intrusive/ellen_bintree_rcu.h>

        // RCU type we use
        typedef cds::urcu::gc< cds::urcu::general_buffered<> >  gpb_rcu;

        // Declare tag structs
        struct int_tag      ;   // int key tag
        struct string_tag   ;   // string key tag

        // Foo struct is derived from two ellen_bintree::node class
        // with different tags
        struct Foo
            : public cds::intrusive::ellen_bintree::node< gpb_rcu, cds::opt::tag< string_tag >>
            , public cds::intrusive::ellen_bintree::node< gpb_rcu, cds::opt::tag< int_tag >>
        {
            std::string m_strKey    ;   // string key
            int         m_nKey      ;   // int key
            //...                       // other non-key data fields
        };

        // String key extractor functor
        struct string_key_extractor
        {
            void operator ()( std::string& key, Foo const& src ) const
            {
                key = src.m_strKey;
            }
        };

        // Int key extractor functor
        struct int_key_extractor
        {
            void operator ()( int& key, Foo const& src ) const
            {
                key = src.m_nKey;
            }
        };

        // String less predicate
        struct string_less {
            bool operator()( Foo const& v1, Foo const& v2 ) const
            { return v1.m_strKey < v2.m_strKey ; }

            bool operator()( Foo const& v, std::string const& s ) const
            { return v.m_strKey < s ; }

            bool operator()( std::string const& s, Foo const& v ) const
            { return s < v.m_strKey ; }

            // Support comparing std::string and char const *
            bool operator()( std::string const& s, char const * p ) const
            { return s.compare(p) < 0 ; }

            bool operator()( Foo const& v, char const * p ) const
            { return v.m_strKey.compare(p) < 0 ; }

            bool operator()( char const * p, std::string const& s ) const
            { return s.compare(p) > 0; }

            bool operator()( char const * p, Foo const& v ) const
            { return v.m_strKey.compare(p) > 0; }
        };

        // Int less predicate
        struct int_less {
            bool operator()( Foo const& v1, Foo const& v2 ) const
            { return v1.m_nKey < v2.m_nKey ; }

            bool operator()( Foo const& v, int n ) const
            { return v.m_nKey < n ; }

            bool operator()( int n, Foo const& v ) const
            { return n < v.m_nKey ; }
        };

        // Type traits for string-indexed set
        struct string_set_traits: public cds::intrusive::ellen_bintree::traits
        {
            typedef cds::intrusive::ellen_bintree::base_hook< cds::opt::gc<gpb_rcu> >, cds::opt::tag< string_tag > > hook;
            typedef string_key_extractor    key_extractor;
            typedef string_less             less;
            typedef cds::atomicity::item_counter item_counter;
        };

        // Type traits for int-indexed set
        struct int_set_traits: public cds::intrusive::ellen_bintree::traits
        {
            typedef cds::intrusive::ellen_bintree::base_hook< cds::opt::gc<gpb_rcu> >, cds::opt::tag< int_tag > > hook;
            typedef int_key_extractor    key_extractor;
            typedef int_less             less;
            typedef cds::atomicity::item_counter item_counter;
        };

        // Declare string-indexed set
        typedef cds::intrusive::EllenBinTree< gpb_rcu, std::string, Foo, string_set_traits >   string_set_type;
        string_set_type theStringSet;

        // Declare int-indexed set
        typedef cds::intrusive::EllenBinTree< gpb_rcu, int, Foo, int_set_traits >   int_set_type;
        int_set_type    theIntSet;

        // Now we can use theStringSet and theIntSet in our program
        // ...
        \endcode
    */
    template < class RCU,
        typename Key,
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = ellen_bintree::traits
#else
        class Traits
#endif
    >
    class EllenBinTree< cds::urcu::gc<RCU>, Key, T, Traits >
    {
    public:
        typedef cds::urcu::gc<RCU>  gc;   ///< RCU Garbage collector
        typedef Key     key_type;         ///< type of a key stored in internal nodes; key is a part of \p value_type
        typedef T       value_type;       ///< type of value stored in the binary tree
        typedef Traits  traits;           ///< Traits template parameter

        typedef typename traits::hook    hook;      ///< hook type
        typedef typename hook::node_type node_type; ///< node type

        typedef typename traits::disposer disposer;   ///< leaf node disposer
        typedef typename traits::back_off back_off;   ///< back-off strategy

    protected:
        //@cond
        typedef ellen_bintree::base_node< gc >                      tree_node; ///< Base type of tree node
        typedef node_type                                           leaf_node; ///< Leaf node type
        typedef ellen_bintree::internal_node< key_type, leaf_node > internal_node; ///< Internal node type
        typedef ellen_bintree::update_desc< leaf_node, internal_node> update_desc; ///< Update descriptor
        typedef typename update_desc::update_ptr                    update_ptr; ///< Marked pointer to update descriptor
        //@endcond

    public:
        using exempt_ptr = cds::urcu::exempt_ptr< gc, value_type, value_type, disposer, void >; ///< pointer to extracted node

    public:
#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator;    ///< key compare functor based on \p Traits::compare and \p Traits::less
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits; ///< Node traits
#   else
        typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;
        struct node_traits: public get_node_traits< value_type, node_type, hook>::type
        {
            static internal_node const& to_internal_node( tree_node const& n )
            {
                assert( n.is_internal());
                return static_cast<internal_node const&>( n );
            }

            static leaf_node const& to_leaf_node( tree_node const& n )
            {
                assert( n.is_leaf());
                return static_cast<leaf_node const&>( n );
            }
        };
#   endif

        typedef typename traits::item_counter  item_counter;   ///< Item counting policy used
        typedef typename traits::memory_model  memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option
        typedef typename traits::stat          stat;           ///< internal statistics type
        typedef typename traits::rcu_check_deadlock rcu_check_deadlock; ///< Deadlock checking policy
        typedef typename traits::key_extractor      key_extractor;      ///< key extracting functor

        typedef typename traits::node_allocator        node_allocator;  ///< Internal node allocator
        typedef typename traits::update_desc_allocator update_desc_allocator; ///< Update descriptor allocator

        typedef typename gc::scoped_lock    rcu_lock;   ///< RCU scoped lock

        static constexpr const bool c_bExtractLockExternal = false; ///< Group of \p extract_xxx functions do not require external locking

    protected:
        //@cond
        typedef ellen_bintree::details::compare< key_type, value_type, key_comparator, node_traits > node_compare;

        typedef cds::urcu::details::check_deadlock_policy< gc, rcu_check_deadlock >   check_deadlock_policy;

        typedef cds::details::Allocator< internal_node, node_allocator >        cxx_node_allocator;
        typedef cds::details::Allocator< update_desc, update_desc_allocator >   cxx_update_desc_allocator;

        struct search_result {
            internal_node *     pGrandParent;
            internal_node *     pParent;
            leaf_node *         pLeaf;
            update_ptr          updParent;
            update_ptr          updGrandParent;
            bool                bRightLeaf      ; // true if pLeaf is right child of pParent, false otherwise
            bool                bRightParent    ; // true if pParent is right child of pGrandParent, false otherwise

            search_result()
                :pGrandParent( nullptr )
                , pParent( nullptr )
                , pLeaf( nullptr )
                ,bRightLeaf( false )
                ,bRightParent( false )
            {}
        };
        //@endcond

    protected:
        //@cond
        internal_node       m_Root;     ///< Tree root node (key= Infinite2)
        leaf_node           m_LeafInf1;
        leaf_node           m_LeafInf2;
        //@endcond

        item_counter        m_ItemCounter;  ///< item counter
        mutable stat        m_Stat;         ///< internal statistics

    protected:
        //@cond
        static void free_leaf_node( value_type* p )
        {
            disposer()( p );
        }
        static void free_leaf_node_void( void* p )
        {
            free_leaf_node( reinterpret_cast<value_type*>( p ));
        }

        internal_node * alloc_internal_node() const
        {
            m_Stat.onInternalNodeCreated();
            internal_node * pNode = cxx_node_allocator().New();
            //pNode->clean();
            return pNode;
        }

        static void free_internal_node( internal_node* pNode )
        {
            cxx_node_allocator().Delete( pNode );
        }
        static void free_internal_node_void( void* pNode )
        {
            free_internal_node( reinterpret_cast<internal_node*>( pNode ));
        }

        struct internal_node_deleter {
            void operator()( internal_node * p) const
            {
                free_internal_node( p );
            }
        };

        typedef std::unique_ptr< internal_node, internal_node_deleter>  unique_internal_node_ptr;

        update_desc * alloc_update_desc() const
        {
            m_Stat.onUpdateDescCreated();
            return cxx_update_desc_allocator().New();
        }

        static void free_update_desc( update_desc* pDesc )
        {
            cxx_update_desc_allocator().Delete( pDesc );
        }
        static void free_update_desc_void( void* pDesc )
        {
            free_update_desc( reinterpret_cast<update_desc*>( pDesc ));
        }

        class retired_list
        {
            update_desc *   pUpdateHead;
            tree_node *     pNodeHead;

        private:
            class forward_iterator
            {
                update_desc *   m_pUpdate;
                tree_node *     m_pNode;

            public:
                forward_iterator( retired_list const& l )
                    : m_pUpdate( l.pUpdateHead )
                    , m_pNode( l.pNodeHead )
                {}

                forward_iterator()
                    : m_pUpdate( nullptr )
                    , m_pNode( nullptr )
                {}

                cds::urcu::retired_ptr operator *()
                {
                    if ( m_pUpdate ) {
                        return cds::urcu::retired_ptr( reinterpret_cast<void *>( m_pUpdate ), free_update_desc_void );
                    }
                    if ( m_pNode ) {
                        if ( m_pNode->is_leaf()) {
                            return cds::urcu::retired_ptr( reinterpret_cast<void *>( node_traits::to_value_ptr( static_cast<leaf_node *>( m_pNode ))),
                                free_leaf_node_void );
                        }
                        else {
                            return cds::urcu::retired_ptr( reinterpret_cast<void *>( static_cast<internal_node *>( m_pNode )),
                                free_internal_node_void );
                        }
                    }
                    return cds::urcu::retired_ptr( nullptr, free_update_desc_void );
                }

                void operator ++()
                {
                    if ( m_pUpdate ) {
                        m_pUpdate = m_pUpdate->pNextRetire;
                        return;
                    }
                    if ( m_pNode )
                        m_pNode = m_pNode->m_pNextRetired;
                }

                friend bool operator ==( forward_iterator const& i1, forward_iterator const& i2 )
                {
                    return i1.m_pUpdate == i2.m_pUpdate && i1.m_pNode == i2.m_pNode;
                }
                friend bool operator !=( forward_iterator const& i1, forward_iterator const& i2 )
                {
                    return !( i1 == i2 );
                }
            };

        public:
            retired_list()
                : pUpdateHead( nullptr )
                , pNodeHead( nullptr )
            {}

            ~retired_list()
            {
                gc::batch_retire( forward_iterator(*this), forward_iterator());
            }

            void push( update_desc * p )
            {
                p->pNextRetire = pUpdateHead;
                pUpdateHead = p;
            }

            void push( tree_node * p )
            {
                p->m_pNextRetired = pNodeHead;
                pNodeHead = p;
            }
        };

        void retire_node( tree_node * pNode, retired_list& rl ) const
        {
            if ( pNode->is_leaf()) {
                assert( static_cast<leaf_node *>( pNode ) != &m_LeafInf1 );
                assert( static_cast<leaf_node *>( pNode ) != &m_LeafInf2 );
            }
            else {
                assert( static_cast<internal_node *>( pNode ) != &m_Root );
                m_Stat.onInternalNodeDeleted();
            }
            rl.push( pNode );
        }

        void retire_update_desc( update_desc * p, retired_list& rl, bool bDirect ) const
        {
            m_Stat.onUpdateDescDeleted();
            if ( bDirect )
                free_update_desc( p );
            else
                rl.push( p );
        }

        void make_empty_tree()
        {
            m_Root.infinite_key( 2 );
            m_LeafInf1.infinite_key( 1 );
            m_LeafInf2.infinite_key( 2 );
            m_Root.m_pLeft.store( &m_LeafInf1, memory_model::memory_order_relaxed );
            m_Root.m_pRight.store( &m_LeafInf2, memory_model::memory_order_release );
        }
        //@endcond

    public:
        /// Default constructor
        EllenBinTree()
        {
            static_assert( !std::is_same< key_extractor, opt::none >::value, "The key extractor option must be specified" );
            make_empty_tree();
        }

        /// Clears the tree
        ~EllenBinTree()
        {
            unsafe_clear();
        }

        /// Inserts new node
        /**
            The function inserts \p val in the tree if it does not contain
            an item with key equal to \p val.

            The function applies RCU lock internally.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert( val, []( value_type& ) {} );
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the tree
            - if inserting is success, calls  \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this tree's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            check_deadlock_policy::check();

            unique_internal_node_ptr pNewInternal;
            retired_list updRetire;
            back_off bkoff;

            {
                rcu_lock l;

                search_result res;
                for ( ;; ) {
                    if ( search( res, val, node_compare())) {
                        if ( pNewInternal.get())
                            m_Stat.onInternalNodeDeleted() ;    // unique_internal_node_ptr deletes internal node
                        m_Stat.onInsertFailed();
                        return false;
                    }

                    if ( res.updGrandParent.bits() == update_desc::Clean && res.updParent.bits() == update_desc::Clean ) {
                        if ( !pNewInternal.get())
                            pNewInternal.reset( alloc_internal_node());

                        if ( try_insert( val, pNewInternal.get(), res, updRetire )) {
                            f( val );
                            pNewInternal.release()  ;   // internal node is linked into the tree and should not be deleted
                            break;
                        }
                    }
                    else
                        help( res.updParent, updRetire );

                    bkoff();
                    m_Stat.onInsertRetry();
                }
            }

            ++m_ItemCounter;
            m_Stat.onInsertSuccess();

            return true;
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the set, then \p val is inserted into the set
            iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor \p func signature is:
            \code
                void func( bool bNew, value_type& item, value_type& val );
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p %update() function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refer to the same thing.

            The functor can change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            RCU \p synchronize method can be called. RCU should not be locked.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            i.e. the node has been inserted or updated,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
        {
            check_deadlock_policy::check();

            unique_internal_node_ptr pNewInternal;
            retired_list updRetire;
            back_off bkoff;

            {
                rcu_lock l;

                search_result res;
                for ( ;; ) {
                    if ( search( res, val, node_compare())) {
                        func( false, *node_traits::to_value_ptr( res.pLeaf ), val );
                        if ( pNewInternal.get())
                            m_Stat.onInternalNodeDeleted() ;    // unique_internal_node_ptr deletes internal node
                        m_Stat.onUpdateExist();
                        return std::make_pair( true, false );
                    }

                    if ( res.updGrandParent.bits() == update_desc::Clean && res.updParent.bits() == update_desc::Clean ) {
                        if ( !bAllowInsert )
                            return std::make_pair( false, false );

                        if ( !pNewInternal.get())
                            pNewInternal.reset( alloc_internal_node());

                        if ( try_insert( val, pNewInternal.get(), res, updRetire )) {
                            func( true, val, val );
                            pNewInternal.release()  ;   // internal node is linked into the tree and should not be deleted
                            break;
                        }
                    }
                    else
                        help( res.updParent, updRetire );

                    bkoff();
                    m_Stat.onUpdateRetry();
                }
            }

            ++m_ItemCounter;
            m_Stat.onUpdateNew();

            return std::make_pair( true, true );
        }
        //@cond
        template <typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Unlinks the item \p val from the tree
        /**
            The function searches the item \p val in the tree and unlink it from the tree
            if it is found and is equal to \p val.

            Difference between \p erase() and \p %unlink() functions: \p %erase() finds <i>a key</i>
            and deletes the item found. \p %unlink() finds an item by key and deletes it
            only if \p val is an item of the tree, i.e. the pointer to item found
            is equal to <tt> &val </tt>.

            RCU \p synchronize method can be called. RCU should not be locked.

            The \ref disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC asynchronously.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            return erase_( val, node_compare(),
                []( value_type const& v, leaf_node const& n ) -> bool { return &v == node_traits::to_value_ptr( n ); },
                [](value_type const&) {} );
        }

        /// Deletes the item from the tree
        /** \anchor cds_intrusive_EllenBinTree_rcu_erase
            The function searches an item with key equal to \p key in the tree,
            unlinks it from the tree, and returns \p true.
            If the item with key equal to \p key is not found the function return \p false.

            Note the \p Traits::less and/or \p Traits::compare predicate should accept a parameter of type \p Q
            that can be not the same as \p value_type.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename Q>
        bool erase( const Q& key )
        {
            return erase_( key, node_compare(),
                []( Q const&, leaf_node const& ) -> bool { return true; },
                [](value_type const&) {} );
        }

        /// Delete the item from the tree with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_EllenBinTree_rcu_erase "erase(Q const&)"
            but \p pred predicate is used for key comparing.
            \p Less has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_rcu_less
            "Predicate requirements".
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less>
        bool erase_with( const Q& key, Less pred )
        {
            CDS_UNUSED( pred );
            typedef ellen_bintree::details::compare<
                key_type,
                value_type,
                opt::details::make_comparator_from_less<Less>,
                node_traits
            > compare_functor;

            return erase_( key, compare_functor(),
                []( Q const&, leaf_node const& ) -> bool { return true; },
                [](value_type const&) {} );
        }

        /// Deletes the item from the tree
        /** \anchor cds_intrusive_EllenBinTree_rcu_erase_func
            The function searches an item with key equal to \p key in the tree,
            call \p f functor with item found, unlinks it from the tree, and returns \p true.
            The \ref disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC asynchronously.

            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode

            If the item with key equal to \p key is not found the function return \p false.

            Note the \p Traits::less and/or \p Traits::compare predicate should accept a parameter of type \p Q
            that can be not the same as \p value_type.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return erase_( key, node_compare(),
                []( Q const&, leaf_node const& ) -> bool { return true; },
                f );
        }

        /// Delete the item from the tree with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_EllenBinTree_rcu_erase_func "erase(Q const&, Func)"
            but \p pred predicate is used for key comparing.
            \p Less has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_rcu_less
            "Predicate requirements".
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            typedef ellen_bintree::details::compare<
                key_type,
                value_type,
                opt::details::make_comparator_from_less<Less>,
                node_traits
            > compare_functor;

            return erase_( key, compare_functor(),
                []( Q const&, leaf_node const& ) -> bool { return true; },
                f );
        }

        /// Extracts an item with minimal key from the tree
        /**
            The function searches an item with minimal key, unlinks it, and returns
            \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the leftmost item.
            If the tree is empty the function returns empty \p exempt_ptr.

            @note Due the concurrent nature of the tree, the function extracts <i>nearly</i> minimum key.
            It means that the function gets leftmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key less than leftmost item's key.
            So, the function returns the item with minimum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not call the disposer for the item found.
            The disposer will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
        */
        exempt_ptr extract_min()
        {
            return exempt_ptr( extract_min_());
        }

        /// Extracts an item with maximal key from the tree
        /**
            The function searches an item with maximal key, unlinks it, and returns
            \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the rightmost item.
            If the tree is empty the function returns empty \p exempt_ptr.

            @note Due the concurrent nature of the tree, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key great than rightmost item's key.
            So, the function returns the item with maximum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not call the disposer for the item found.
            The disposer will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
        */
        exempt_ptr extract_max()
        {
            return exempt_ptr( extract_max_());
        }

        /// Extracts an item from the tree
        /** \anchor cds_intrusive_EllenBinTree_rcu_extract
            The function searches an item with key equal to \p key in the tree,
            unlinks it, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to an item found.
            If the item with the key equal to \p key is not found the function returns empty \p exempt_ptr.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not call the disposer for the item found.
            The disposer will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
        */
        template <typename Q>
        exempt_ptr extract( Q const& key )
        {
            return exempt_ptr( extract_( key, node_compare()));
        }

        /// Extracts an item from the set using \p pred for searching
        /**
            The function is an analog of \p extract(Q const&) but \p pred is used for key compare.
            \p Less has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_rcu_less
            "predicate requirements".
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            return exempt_ptr( extract_with_( key, pred ));
        }

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            The function applies RCU lock internally.
        */
        template <typename Q>
        bool contains( Q const& key ) const
        {
            rcu_lock l;
            search_result    res;
            if ( search( res, key, node_compare())) {
                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key ) const
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the set contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_rcu_less
            "Predicate requirements".
            \p Less must imply the same element order as the comparator used for building the set.
            \p pred should accept arguments of type \p Q, \p key_type, \p value_type in any combination.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            typedef ellen_bintree::details::compare<
                key_type,
                value_type,
                opt::details::make_comparator_from_less<Less>,
                node_traits
            > compare_functor;

            rcu_lock l;
            search_result    res;
            if ( search( res, key, compare_functor())) {
                m_Stat.onFindSuccess();
                return true;
            }
            m_Stat.onFindFailed();
            return false;
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred ) const
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds the key \p key
        /** @anchor cds_intrusive_EllenBinTree_rcu_find_func
            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor can change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the tree \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The function applies RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f ) const
        {
            return find_( key, f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f ) const
        {
            return find_( key, f );
        }
        //@endcond

        /// Finds the key \p key with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_EllenBinTree_rcu_find_func "find(Q&, Func)"
            but \p pred is used for key comparison.
            \p Less functor has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_rcu_less
            "Predicate requirements".
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f ) const
        {
            return find_with_( key, pred, f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            return find_with_( key, pred, f );
        }
        //@endcond

        /// Finds \p key and return the item found
        /** \anchor cds_intrusive_EllenBinTree_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.

            RCU should be locked before call the function.
            Returned pointer is valid while RCU is locked.
        */
        template <typename Q>
        value_type * get( Q const& key ) const
        {
            return get_( key, node_compare());
        }

        /// Finds \p key with \p pred predicate and return the item found
        /**
            The function is an analog of \ref cds_intrusive_EllenBinTree_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less>
        value_type * get_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            typedef ellen_bintree::details::compare<
                key_type,
                value_type,
                opt::details::make_comparator_from_less<Less>,
                node_traits
            > compare_functor;

            return get_( key, compare_functor());
        }

        /// Checks if the tree is empty
        bool empty() const
        {
            return m_Root.m_pLeft.load( memory_model::memory_order_relaxed )->is_leaf();
        }

        /// Clears the tree (thread safe, not atomic)
        /**
            The function unlink all items from the tree.
            The function is thread safe but not atomic: in multi-threaded environment with parallel insertions
            this sequence
            \code
            set.clear();
            assert( set.empty());
            \endcode
            the assertion could be raised.

            For each leaf the \ref disposer will be called after unlinking.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        void clear()
        {
            for ( exempt_ptr ep = extract_min(); !ep.empty(); ep = extract_min())
                ep.release();
        }

        /// Clears the tree (not thread safe)
        /**
            This function is not thread safe and may be called only when no other thread deals with the tree.
            The function is used in the tree destructor.
        */
        void unsafe_clear()
        {
            rcu_lock l;

            while ( true ) {
                internal_node * pParent = nullptr;
                internal_node * pGrandParent = nullptr;
                tree_node *     pLeaf = const_cast<internal_node *>( &m_Root );

                // Get leftmost leaf
                while ( pLeaf->is_internal()) {
                    pGrandParent = pParent;
                    pParent = static_cast<internal_node *>( pLeaf );
                    pLeaf = pParent->m_pLeft.load( memory_model::memory_order_relaxed );
                }

                if ( pLeaf->infinite_key()) {
                    // The tree is empty
                    return;
                }

                // Remove leftmost leaf and its parent node
                assert( pGrandParent );
                assert( pParent );
                assert( pLeaf->is_leaf());

                pGrandParent->m_pLeft.store( pParent->m_pRight.load( memory_model::memory_order_relaxed ), memory_model::memory_order_relaxed );
                free_leaf_node( node_traits::to_value_ptr( static_cast<leaf_node *>( pLeaf )));
                free_internal_node( pParent );
            }
        }

        /// Returns item count in the tree
        /**
            Only leaf nodes containing user data are counted.

            The value returned depends on item counter type provided by \p Traits template parameter.
            If it is \p atomicity::empty_item_counter this function always returns 0.

            The function is not suitable for checking the tree emptiness, use \p empty()
            member function for that.
        */
        size_t size() const
        {
            return m_ItemCounter;
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return m_Stat;
        }

        /// Checks internal consistency (not atomic, not thread-safe)
        /**
            The debugging function to check internal consistency of the tree.
        */
        bool check_consistency() const
        {
            return check_consistency( &m_Root );
        }

    protected:
        //@cond

        bool check_consistency( internal_node const * pRoot ) const
        {
            tree_node * pLeft  = pRoot->m_pLeft.load( atomics::memory_order_relaxed );
            tree_node * pRight = pRoot->m_pRight.load( atomics::memory_order_relaxed );
            assert( pLeft );
            assert( pRight );

            if ( node_compare()( *pLeft, *pRoot ) < 0
                && node_compare()( *pRoot, *pRight ) <= 0
                && node_compare()( *pLeft, *pRight ) < 0 )
            {
                bool bRet = true;
                if ( pLeft->is_internal())
                    bRet = check_consistency( static_cast<internal_node *>( pLeft ));
                assert( bRet );

                if ( bRet && pRight->is_internal())
                    bRet = bRet && check_consistency( static_cast<internal_node *>( pRight ));
                assert( bRet );

                return bRet;
            }
            return false;
        }

        void help( update_ptr /*pUpdate*/, retired_list& /*rl*/ )
        {
            /*
            switch ( pUpdate.bits()) {
                case update_desc::IFlag:
                    help_insert( pUpdate.ptr());
                    m_Stat.onHelpInsert();
                    break;
                case update_desc::DFlag:
                    //help_delete( pUpdate.ptr(), rl );
                    //m_Stat.onHelpDelete();
                    break;
                case update_desc::Mark:
                    //help_marked( pUpdate.ptr());
                    //m_Stat.onHelpMark();
                    break;
            }
            */
        }

        void help_insert( update_desc * pOp )
        {
            assert( gc::is_locked());

            tree_node * pLeaf = static_cast<tree_node *>( pOp->iInfo.pLeaf );
            if ( pOp->iInfo.bRightLeaf ) {
                pOp->iInfo.pParent->m_pRight.compare_exchange_strong( pLeaf, static_cast<tree_node *>( pOp->iInfo.pNew ),
                    memory_model::memory_order_release, atomics::memory_order_relaxed );
            }
            else {
                pOp->iInfo.pParent->m_pLeft.compare_exchange_strong( pLeaf, static_cast<tree_node *>( pOp->iInfo.pNew ),
                    memory_model::memory_order_release, atomics::memory_order_relaxed );
            }

            update_ptr cur( pOp, update_desc::IFlag );
            pOp->iInfo.pParent->m_pUpdate.compare_exchange_strong( cur, pOp->iInfo.pParent->null_update_desc(),
                      memory_model::memory_order_release, atomics::memory_order_relaxed );
        }

        bool check_delete_precondition( search_result& res )
        {
            assert( res.pGrandParent != nullptr );

            return
                static_cast<internal_node *>( res.pGrandParent->get_child( res.bRightParent, memory_model::memory_order_relaxed )) == res.pParent
             && static_cast<leaf_node *>( res.pParent->get_child( res.bRightLeaf, memory_model::memory_order_relaxed )) == res.pLeaf;
        }

        bool help_delete( update_desc * pOp, retired_list& rl )
        {
            assert( gc::is_locked());

            update_ptr pUpdate( pOp->dInfo.pUpdateParent );
            update_ptr pMark( pOp, update_desc::Mark );
            if ( pOp->dInfo.pParent->m_pUpdate.compare_exchange_strong( pUpdate, pMark,
                    memory_model::memory_order_acquire, atomics::memory_order_relaxed ))
            {
                help_marked( pOp );
                retire_node( pOp->dInfo.pParent, rl );
                // For extract operations the leaf should NOT be disposed
                if ( pOp->dInfo.bDisposeLeaf )
                    retire_node( pOp->dInfo.pLeaf, rl );
                retire_update_desc( pOp, rl, false );

                return true;
            }
            else if ( pUpdate == pMark ) {
                // some other thread is processing help_marked()
                help_marked( pOp );
                m_Stat.onHelpMark();
                return true;
            }
            else {
                // pUpdate has been changed by CAS
                help( pUpdate, rl );

                // Undo grandparent dInfo
                update_ptr pDel( pOp, update_desc::DFlag );
                if ( pOp->dInfo.pGrandParent->m_pUpdate.compare_exchange_strong( pDel, pOp->dInfo.pGrandParent->null_update_desc(),
                    memory_model::memory_order_release, atomics::memory_order_relaxed ))
                {
                    retire_update_desc( pOp, rl, false );
                }
                return false;
            }
        }

        void help_marked( update_desc * pOp )
        {
            assert( gc::is_locked());

            tree_node * p = pOp->dInfo.pParent;
            if ( pOp->dInfo.bRightParent ) {
                pOp->dInfo.pGrandParent->m_pRight.compare_exchange_strong( p,
                    pOp->dInfo.pParent->get_child( !pOp->dInfo.bRightLeaf, memory_model::memory_order_acquire ),
                    memory_model::memory_order_release, atomics::memory_order_relaxed );
            }
            else {
                pOp->dInfo.pGrandParent->m_pLeft.compare_exchange_strong( p,
                    pOp->dInfo.pParent->get_child( !pOp->dInfo.bRightLeaf, memory_model::memory_order_acquire ),
                    memory_model::memory_order_release, atomics::memory_order_relaxed );
            }

            update_ptr upd( pOp, update_desc::DFlag );
            pOp->dInfo.pGrandParent->m_pUpdate.compare_exchange_strong( upd, pOp->dInfo.pGrandParent->null_update_desc(),
                memory_model::memory_order_release, atomics::memory_order_relaxed );
        }

        template <typename KeyValue, typename Compare>
        bool search( search_result& res, KeyValue const& key, Compare cmp ) const
        {
            assert( gc::is_locked());

            internal_node * pParent;
            internal_node * pGrandParent = nullptr;
            tree_node *     pLeaf;
            update_ptr      updParent;
            update_ptr      updGrandParent;
            bool bRightLeaf;
            bool bRightParent = false;

            int nCmp = 0;

        retry:
            pParent = nullptr;
            pLeaf = const_cast<internal_node *>( &m_Root );
            updParent = nullptr;
            bRightLeaf = false;
            while ( pLeaf->is_internal()) {
                pGrandParent = pParent;
                pParent = static_cast<internal_node *>( pLeaf );
                bRightParent = bRightLeaf;
                updGrandParent = updParent;
                updParent = pParent->m_pUpdate.load( memory_model::memory_order_acquire );

                switch ( updParent.bits()) {
                    case update_desc::DFlag:
                    case update_desc::Mark:
                        m_Stat.onSearchRetry();
                        goto retry;
                }

                nCmp = cmp( key, *pParent );
                bRightLeaf = nCmp >= 0;
                pLeaf = pParent->get_child( nCmp >= 0, memory_model::memory_order_acquire );
            }

            assert( pLeaf->is_leaf());
            nCmp = cmp( key, *static_cast<leaf_node *>(pLeaf));

            res.pGrandParent    = pGrandParent;
            res.pParent         = pParent;
            res.pLeaf           = static_cast<leaf_node *>( pLeaf );
            res.updParent       = updParent;
            res.updGrandParent  = updGrandParent;
            res.bRightParent    = bRightParent;
            res.bRightLeaf      = bRightLeaf;

            return nCmp == 0;
        }

        bool search_min( search_result& res ) const
        {
            assert( gc::is_locked());

            internal_node * pParent;
            internal_node * pGrandParent = nullptr;
            tree_node *     pLeaf;
            update_ptr      updParent;
            update_ptr      updGrandParent;

        retry:
            pParent = nullptr;
            pLeaf = const_cast<internal_node *>( &m_Root );
            while ( pLeaf->is_internal()) {
                pGrandParent = pParent;
                pParent = static_cast<internal_node *>( pLeaf );
                updGrandParent = updParent;
                updParent = pParent->m_pUpdate.load( memory_model::memory_order_acquire );

                switch ( updParent.bits()) {
                    case update_desc::DFlag:
                    case update_desc::Mark:
                        m_Stat.onSearchRetry();
                        goto retry;
                }

                pLeaf = pParent->m_pLeft.load( memory_model::memory_order_acquire );
            }

            if ( pLeaf->infinite_key())
                return false;

            res.pGrandParent    = pGrandParent;
            res.pParent         = pParent;
            assert( pLeaf->is_leaf());
            res.pLeaf           = static_cast<leaf_node *>( pLeaf );
            res.updParent       = updParent;
            res.updGrandParent  = updGrandParent;
            res.bRightParent    = false;
            res.bRightLeaf      = false;

            return true;
        }

        bool search_max( search_result& res ) const
        {
            assert( gc::is_locked());

            internal_node * pParent;
            internal_node * pGrandParent = nullptr;
            tree_node *     pLeaf;
            update_ptr      updParent;
            update_ptr      updGrandParent;
            bool bRightLeaf;
            bool bRightParent = false;

        retry:
            pParent = nullptr;
            pLeaf = const_cast<internal_node *>( &m_Root );
            bRightLeaf = false;
            while ( pLeaf->is_internal()) {
                pGrandParent = pParent;
                pParent = static_cast<internal_node *>( pLeaf );
                bRightParent = bRightLeaf;
                updGrandParent = updParent;
                updParent = pParent->m_pUpdate.load( memory_model::memory_order_acquire );

                switch ( updParent.bits()) {
                    case update_desc::DFlag:
                    case update_desc::Mark:
                        m_Stat.onSearchRetry();
                        goto retry;
                }

                bRightLeaf = !pParent->infinite_key();
                pLeaf = pParent->get_child( bRightLeaf, memory_model::memory_order_acquire );
            }

            if ( pLeaf->infinite_key())
                return false;

            res.pGrandParent    = pGrandParent;
            res.pParent         = pParent;
            assert( pLeaf->is_leaf());
            res.pLeaf           = static_cast<leaf_node *>( pLeaf );
            res.updParent       = updParent;
            res.updGrandParent  = updGrandParent;
            res.bRightParent    = bRightParent;
            res.bRightLeaf      = bRightLeaf;

            return true;
        }

        template <typename Q, typename Compare, typename Equal, typename Func>
        bool erase_( Q const& val, Compare cmp, Equal eq, Func f )
        {
            check_deadlock_policy::check();

            retired_list updRetire;
            update_desc * pOp = nullptr;
            search_result res;
            back_off bkoff;

            {
                rcu_lock l;
                for ( ;; ) {
                    if ( !search( res, val, cmp ) || !eq( val, *res.pLeaf )) {
                        if ( pOp )
                            retire_update_desc( pOp, updRetire, false );
                        m_Stat.onEraseFailed();
                        return false;
                    }

                    if ( res.updGrandParent.bits() != update_desc::Clean )
                        help( res.updGrandParent, updRetire );
                    else if ( res.updParent.bits() != update_desc::Clean )
                        help( res.updParent, updRetire );
                    else {
                        if ( !pOp )
                            pOp = alloc_update_desc();
                        if ( check_delete_precondition( res )) {
                            pOp->dInfo.pGrandParent = res.pGrandParent;
                            pOp->dInfo.pParent = res.pParent;
                            pOp->dInfo.pLeaf = res.pLeaf;
                            pOp->dInfo.bDisposeLeaf = true;
                            pOp->dInfo.pUpdateParent = res.updParent.ptr();
                            pOp->dInfo.bRightParent = res.bRightParent;
                            pOp->dInfo.bRightLeaf = res.bRightLeaf;

                            update_ptr updGP( res.updGrandParent.ptr());
                            if ( res.pGrandParent->m_pUpdate.compare_exchange_strong( updGP, update_ptr( pOp, update_desc::DFlag ),
                                memory_model::memory_order_acq_rel, atomics::memory_order_acquire ))
                            {
                                if ( help_delete( pOp, updRetire )) {
                                    // res.pLeaf is not deleted yet since RCU is blocked
                                    f( *node_traits::to_value_ptr( res.pLeaf ));
                                    break;
                                }
                                pOp = nullptr;
                            }
                            else {
                                // updGP has been changed by CAS
                                help( updGP, updRetire );
                            }
                        }
                    }

                    bkoff();
                    m_Stat.onEraseRetry();
                }
            }

            --m_ItemCounter;
            m_Stat.onEraseSuccess();
            return true;
        }

        template <typename Q, typename Less>
        value_type * extract_with_( Q const& val, Less /*pred*/ )
        {
            typedef ellen_bintree::details::compare<
                key_type,
                value_type,
                opt::details::make_comparator_from_less<Less>,
                node_traits
            > compare_functor;

            return extract_( val, compare_functor());
        }

        template <typename Q, typename Compare>
        value_type * extract_( Q const& val, Compare cmp )
        {
            check_deadlock_policy::check();

            retired_list updRetire;
            update_desc * pOp = nullptr;
            search_result res;
            back_off bkoff;
            value_type * pResult;

            {
                rcu_lock l;
                for ( ;; ) {
                    if ( !search( res, val, cmp )) {
                        if ( pOp )
                            retire_update_desc( pOp, updRetire, false );
                        m_Stat.onEraseFailed();
                        return nullptr;
                    }

                    if ( res.updGrandParent.bits() != update_desc::Clean )
                        help( res.updGrandParent, updRetire );
                    else if ( res.updParent.bits() != update_desc::Clean )
                        help( res.updParent, updRetire );
                    else {
                        if ( !pOp )
                            pOp = alloc_update_desc();
                        if ( check_delete_precondition( res )) {
                            pOp->dInfo.pGrandParent = res.pGrandParent;
                            pOp->dInfo.pParent = res.pParent;
                            pOp->dInfo.pLeaf = res.pLeaf;
                            pOp->dInfo.bDisposeLeaf = false;
                            pOp->dInfo.pUpdateParent = res.updParent.ptr();
                            pOp->dInfo.bRightParent = res.bRightParent;
                            pOp->dInfo.bRightLeaf = res.bRightLeaf;

                            update_ptr updGP( res.updGrandParent.ptr());
                            if ( res.pGrandParent->m_pUpdate.compare_exchange_strong( updGP, update_ptr( pOp, update_desc::DFlag ),
                                memory_model::memory_order_acq_rel, atomics::memory_order_acquire ))
                            {
                                if ( help_delete( pOp, updRetire )) {
                                    pResult = node_traits::to_value_ptr( res.pLeaf );
                                    break;
                                }
                                pOp = nullptr;
                            }
                            else {
                                // updGP has been changed by CAS
                                help( updGP, updRetire );
                            }
                        }
                    }

                    bkoff();
                    m_Stat.onEraseRetry();
                }
            }

            --m_ItemCounter;
            m_Stat.onEraseSuccess();
            return pResult;
        }


        value_type * extract_max_()
        {
            check_deadlock_policy::check();

            retired_list updRetire;
            update_desc * pOp = nullptr;
            search_result res;
            back_off bkoff;
            value_type * pResult;

            {
                rcu_lock l;
                for ( ;; ) {
                    if ( !search_max( res )) {
                        // Tree is empty
                        if ( pOp )
                            retire_update_desc( pOp, updRetire, false );
                        m_Stat.onExtractMaxFailed();
                        return nullptr;
                    }

                    if ( res.updGrandParent.bits() != update_desc::Clean )
                        help( res.updGrandParent, updRetire );
                    else if ( res.updParent.bits() != update_desc::Clean )
                        help( res.updParent, updRetire );
                    else {
                        if ( !pOp )
                            pOp = alloc_update_desc();
                        if ( check_delete_precondition( res )) {
                            pOp->dInfo.pGrandParent = res.pGrandParent;
                            pOp->dInfo.pParent = res.pParent;
                            pOp->dInfo.pLeaf = res.pLeaf;
                            pOp->dInfo.bDisposeLeaf = false;
                            pOp->dInfo.pUpdateParent = res.updParent.ptr();
                            pOp->dInfo.bRightParent = res.bRightParent;
                            pOp->dInfo.bRightLeaf = res.bRightLeaf;

                            update_ptr updGP( res.updGrandParent.ptr());
                            if ( res.pGrandParent->m_pUpdate.compare_exchange_strong( updGP, update_ptr( pOp, update_desc::DFlag ),
                                memory_model::memory_order_acq_rel, atomics::memory_order_acquire ))
                            {
                                if ( help_delete( pOp, updRetire )) {
                                    pResult = node_traits::to_value_ptr( res.pLeaf );
                                    break;
                                }
                                pOp = nullptr;
                            }
                            else {
                                // updGP has been changed by CAS
                                help( updGP, updRetire );
                            }
                        }
                    }

                    bkoff();
                    m_Stat.onExtractMaxRetry();
                }
            }

            --m_ItemCounter;
            m_Stat.onExtractMaxSuccess();
            return pResult;
        }

        value_type * extract_min_()
        {
            check_deadlock_policy::check();

            retired_list updRetire;
            update_desc * pOp = nullptr;
            search_result res;
            back_off bkoff;
            value_type * pResult;

            {
                rcu_lock l;
                for ( ;; ) {
                    if ( !search_min( res )) {
                        // Tree is empty
                        if ( pOp )
                            retire_update_desc( pOp, updRetire, false );
                        m_Stat.onExtractMinFailed();
                        return nullptr;
                    }

                    if ( res.updGrandParent.bits() != update_desc::Clean )
                        help( res.updGrandParent, updRetire );
                    else if ( res.updParent.bits() != update_desc::Clean )
                        help( res.updParent, updRetire );
                    else {
                        if ( !pOp )
                            pOp = alloc_update_desc();
                        if ( check_delete_precondition( res )) {
                            pOp->dInfo.pGrandParent = res.pGrandParent;
                            pOp->dInfo.pParent = res.pParent;
                            pOp->dInfo.pLeaf = res.pLeaf;
                            pOp->dInfo.bDisposeLeaf = false;
                            pOp->dInfo.pUpdateParent = res.updParent.ptr();
                            pOp->dInfo.bRightParent = res.bRightParent;
                            pOp->dInfo.bRightLeaf = res.bRightLeaf;

                            update_ptr updGP( res.updGrandParent.ptr());
                            if ( res.pGrandParent->m_pUpdate.compare_exchange_strong( updGP, update_ptr( pOp, update_desc::DFlag ),
                                memory_model::memory_order_acq_rel, atomics::memory_order_acquire ))
                            {
                                if ( help_delete( pOp, updRetire )) {
                                    pResult = node_traits::to_value_ptr( res.pLeaf );
                                    break;
                                }
                                pOp = nullptr;
                            }
                            else {
                                // updGP has been changed by CAS
                                help( updGP, updRetire );
                            }
                        }
                    }

                    bkoff();
                    m_Stat.onExtractMinRetry();
                }
            }

            --m_ItemCounter;
            m_Stat.onExtractMinSuccess();
            return pResult;
        }

        template <typename Q, typename Less, typename Func>
        bool find_with_( Q& val, Less /*pred*/, Func f ) const
        {
            typedef ellen_bintree::details::compare<
                key_type,
                value_type,
                opt::details::make_comparator_from_less<Less>,
                node_traits
            > compare_functor;

            rcu_lock l;
            search_result    res;
            if ( search( res, val, compare_functor())) {
                assert( res.pLeaf );
                f( *node_traits::to_value_ptr( res.pLeaf ), val );

                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Func>
        bool find_( Q& key, Func f ) const
        {
            rcu_lock l;
            search_result    res;
            if ( search( res, key, node_compare())) {
                assert( res.pLeaf );
                f( *node_traits::to_value_ptr( res.pLeaf ), key );

                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Compare>
        value_type * get_( Q const& key, Compare cmp ) const
        {
            assert( gc::is_locked());

            search_result    res;
            if ( search( res, key, cmp )) {
                m_Stat.onFindSuccess();
                return node_traits::to_value_ptr( res.pLeaf );
            }

            m_Stat.onFindFailed();
            return nullptr;
        }


        bool try_insert( value_type& val, internal_node * pNewInternal, search_result& res, retired_list& updRetire )
        {
            assert( gc::is_locked());
            assert( res.updParent.bits() == update_desc::Clean );

            // check search result
            if ( static_cast<leaf_node *>( res.pParent->get_child( res.bRightLeaf, memory_model::memory_order_relaxed )) == res.pLeaf ) {
                leaf_node * pNewLeaf = node_traits::to_node_ptr( val );

                int nCmp = node_compare()( val, *res.pLeaf );
                if ( nCmp < 0 ) {
                    if ( res.pGrandParent ) {
                        pNewInternal->infinite_key( 0 );
                        key_extractor()( pNewInternal->m_Key, *node_traits::to_value_ptr( res.pLeaf ));
                        assert( !res.pLeaf->infinite_key());
                    }
                    else {
                        assert( res.pLeaf->infinite_key() == tree_node::key_infinite1 );
                        pNewInternal->infinite_key( 1 );
                    }
                    pNewInternal->m_pLeft.store( static_cast<tree_node *>(pNewLeaf), memory_model::memory_order_relaxed );
                    pNewInternal->m_pRight.store( static_cast<tree_node *>(res.pLeaf), memory_model::memory_order_relaxed );
                }
                else {
                    assert( !res.pLeaf->is_internal());
                    pNewInternal->infinite_key( 0 );

                    key_extractor()( pNewInternal->m_Key, val );
                    pNewInternal->m_pLeft.store( static_cast<tree_node *>(res.pLeaf), memory_model::memory_order_relaxed );
                    pNewInternal->m_pRight.store( static_cast<tree_node *>(pNewLeaf), memory_model::memory_order_relaxed );
                    assert( !res.pLeaf->infinite_key());
                }

                update_desc * pOp = alloc_update_desc();

                pOp->iInfo.pParent = res.pParent;
                pOp->iInfo.pNew = pNewInternal;
                pOp->iInfo.pLeaf = res.pLeaf;
                pOp->iInfo.bRightLeaf = res.bRightLeaf;

                update_ptr updCur( res.updParent.ptr());
                if ( res.pParent->m_pUpdate.compare_exchange_strong( updCur, update_ptr( pOp, update_desc::IFlag ),
                    memory_model::memory_order_acq_rel, atomics::memory_order_acquire ))
                {
                    // do insert
                    help_insert( pOp );
                    retire_update_desc( pOp, updRetire, false );
                    return true;
                }
                else {
                    // updCur has been updated by CAS
                    help( updCur, updRetire );
                    retire_update_desc( pOp, updRetire, true );
                }
            }
            return false;
        }

        //@endcond
    };

}} // namespace cds::intrusive

#endif  // #ifndef CDSLIB_INTRUSIVE_ELLEN_BINTREE_RCU_H
