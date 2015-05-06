//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_LAZY_KVLIST_NOGC_H
#define CDSLIB_CONTAINER_LAZY_KVLIST_NOGC_H

#include <memory>
#include <cds/container/details/lazy_list_base.h>
#include <cds/intrusive/lazy_list_nogc.h>
#include <cds/container/details/make_lazy_kvlist.h>

namespace cds { namespace container {

    /// Lazy ordered list (key-value pair, template specialization for gc::nogc)
    /** @ingroup cds_nonintrusive_list
        @anchor cds_nonintrusive_LazyKVList_nogc

        This specialization is append-only list when no item
        reclamation may be performed. The class does not support deleting of list item.

        @copydetails cds_nonintrusive_LazyList_gc
    */
    template <
        typename Key,
        typename Value,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = lazy_list::traits
#else
        typename Traits
#endif
    >
    class LazyKVList<gc::nogc, Key, Value, Traits>:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::LazyList< gc::nogc, implementation_defined, Traits >
#else
        protected details::make_lazy_kvlist< cds::gc::nogc, Key, Value, Traits >::type
#endif
    {
        //@cond
        typedef details::make_lazy_kvlist< cds::gc::nogc, Key, Value, Traits > maker;
        typedef typename maker::type  base_class;
        //@endcond

    public:
        typedef Traits traits;    ///< List traits
        typedef cds::gc::nogc gc; ///< Garbage collector
#ifdef CDS_DOXYGEN_INVOKED
        typedef Key                                 key_type        ;   ///< Key type
        typedef Value                               mapped_type     ;   ///< Type of value stored in the list
        typedef std::pair<key_type const, mapped_type> value_type   ;   ///< key/value pair stored in the list
#else
        typedef typename maker::key_type    key_type;
        typedef typename maker::mapped_type mapped_type;
        typedef typename maker::value_type  value_type;
#endif
        typedef typename base_class::back_off     back_off;       ///< Back-off strategy used
        typedef typename maker::allocator_type    allocator_type; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename base_class::item_counter item_counter;   ///< Item counting policy used
        typedef typename maker::key_comparator    key_comparator; ///< key comparison functor
        typedef typename base_class::memory_model memory_model;   ///< Memory ordering. See cds::opt::memory_model option
        static CDS_CONSTEXPR bool const c_bSort = base_class::c_bSort; ///< List type: ordered (\p true) or unordered (\p false)

    protected:
        //@cond
        typedef typename base_class::value_type     node_type;
        typedef typename maker::cxx_allocator       cxx_allocator;
        typedef typename maker::node_deallocator    node_deallocator;
        typedef typename base_class::key_comparator intrusive_key_comparator;
        typedef typename base_class::node_type      head_type;
        //@endcond

    protected:
        //@cond
        template <typename K>
        static node_type * alloc_node(const K& key)
        {
            return cxx_allocator().New( key );
        }

        template <typename K, typename V>
        static node_type * alloc_node( const K& key, const V& val )
        {
            return cxx_allocator().New( key, val );
        }

        template <typename... Args>
        static node_type * alloc_node( Args&&... args )
        {
            return cxx_allocator().MoveNew( std::forward<Args>(args)... );
        }

        static void free_node( node_type * pNode )
        {
            cxx_allocator().Delete( pNode );
        }

        struct node_disposer {
            void operator()( node_type * pNode )
            {
                free_node( pNode );
            }
        };
        typedef std::unique_ptr< node_type, node_disposer >     scoped_node_ptr;

        head_type& head()
        {
            return base_class::m_Head;
        }

        head_type const& head() const
        {
            return base_class::m_Head;
        }

        head_type& tail()
        {
            return base_class::m_Tail;
        }

        head_type const& tail() const
        {
            return base_class::m_Tail;
        }
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type: protected base_class::template iterator_type<IsConst>
        {
            typedef typename base_class::template iterator_type<IsConst>    iterator_base;

            iterator_type( head_type const& refNode )
                : iterator_base( const_cast<head_type *>( &refNode ))
            {}

            explicit iterator_type( const iterator_base& it )
                : iterator_base( it )
            {}

            friend class LazyKVList;

        protected:
            explicit iterator_type( node_type& pNode )
                : iterator_base( &pNode )
            {}

        public:
            typedef typename cds::details::make_const_type<mapped_type, IsConst>::reference  value_ref;
            typedef typename cds::details::make_const_type<mapped_type, IsConst>::pointer    value_ptr;

            typedef typename cds::details::make_const_type<value_type,  IsConst>::reference  pair_ref;
            typedef typename cds::details::make_const_type<value_type,  IsConst>::pointer    pair_ptr;

            iterator_type()
                : iterator_base()
            {}

            iterator_type( const iterator_type& src )
                : iterator_base( src )
            {}

            key_type const& key() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                assert( p != nullptr );
                return p->m_Data.first;
            }

            value_ref val() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                assert( p != nullptr );
                return p->m_Data.second;
            }

            pair_ptr operator ->() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                return p ? &(p->m_Data) : nullptr;
            }

            pair_ref operator *() const
            {
                typename iterator_base::value_ref p = iterator_base::operator *();
                return p.m_Data;
            }

            /// Pre-increment
            iterator_type& operator ++()
            {
                iterator_base::operator ++();
                return *this;
            }

            /// Post-increment
            iterator_type operator ++(int)
            {
                return iterator_base::operator ++(0);
            }

            template <bool C>
            bool operator ==(iterator_type<C> const& i ) const
            {
                return iterator_base::operator ==(i);
            }
            template <bool C>
            bool operator !=(iterator_type<C> const& i ) const
            {
                return iterator_base::operator !=(i);
            }
        };
        //@endcond

    public:
        /// Forward iterator
        /**
            The forward iterator for lazy list based on gc::nogc has pre- and post-increment operators.

            The iterator interface to access item data:
            - <tt> operator -> </tt> - returns a pointer to \ref value_type for iterator
            - <tt> operator *</tt> - returns a reference (a const reference for \p const_iterator) to \ref value_type for iterator
            - <tt> const key_type& key() </tt> - returns a key reference for iterator
            - <tt> mapped_type& val() </tt> - retuns a value reference for iterator (const reference for \p const_iterator)

            For both functions the iterator should not be equal to <tt> end() </tt>
        */
        typedef iterator_type<false>    iterator;

        /// Const forward iterator
        /**
            For iterator's features and requirements see \ref iterator
        */
        typedef iterator_type<true>     const_iterator;

        /// Returns a forward iterator addressing the first element in a list
        /**
            For empty list \code begin() == end() \endcode
        */
        iterator begin()
        {
            iterator it( head() );
            ++it ;  // skip dummy head
            return it;
        }

        /// Returns an iterator that addresses the location succeeding the last element in a list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            Internally, <tt>end</tt> returning value equals to nullptr.

            The returned value can be used only to control reaching the end of the list.
            For empty list \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( tail());
        }

        /// Returns a forward const iterator addressing the first element in a list
        //@{
        const_iterator begin() const
        {
            const_iterator it( head() );
            ++it ;  // skip dummy head
            return it;
        }
        const_iterator cbegin() const
        {
            const_iterator it( head() );
            ++it ;  // skip dummy head
            return it;
        }
        //@}

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        //@{
        const_iterator end() const
        {
            return const_iterator( tail());
        }
        const_iterator cend() const
        {
            return const_iterator( tail());
        }
        //@}

    protected:
        //@cond
        iterator node_to_iterator( node_type * pNode )
        {
            if ( pNode )
                return iterator( *pNode );
            return end();
        }
        //@endcond

    public:
        /// Default constructor
        LazyKVList()
        {}

        /// Desctructor clears the list
        ~LazyKVList()
        {
            clear();
        }

        /// Inserts new node with key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the list.

            Preconditions:
            - The \ref key_type should be constructible from value of type \p K.
                In trivial case, \p K is equal to \ref key_type.
            - The \ref mapped_type should be default-constructible.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K>
        iterator insert( const K& key )
        {
            return node_to_iterator( insert_at( head(), key ));
        }

        /// Inserts new node with a key and a value
        /**
            The function creates a node with \p key and value \p val, and then inserts the node created into the list.

            Preconditions:
            - The \ref key_type should be constructible from \p key of type \p K.
            - The \ref mapped_type should be constructible from \p val of type \p V.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K, typename V>
        iterator insert( const K& key, const V& val )
        {
            // We cannot use insert with functor here
            // because we cannot lock inserted node for updating
            // Therefore, we use separate function
            return node_to_iterator( insert_at( head(), key, val ));
        }

        /// Inserts new node and initialize it by a functor
        /**
            This function inserts new node with key \p key and if inserting is successful then it calls
            \p func functor with signature
            \code void func( value_type& item ) ; endcode
            or
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode

            The argument \p item of user-defined functor \p func is the reference
            to the list's item inserted. <tt>item.second</tt> is a reference to item's value that may be changed.
            The user-defined functor is called only if the inserting is successful.

            The key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the list;
            - if inserting is successful, initialize the value of item by calling \p f functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K, typename Func>
        iterator insert_with( const K& key, Func func )
        {
            return node_to_iterator( insert_with_at( head(), key, func ));
        }

        /// Ensures that the key \p key exists in the list
        /**
            The operation inserts new item if the key \p key is not found in the list.
            Otherwise, the function returns an iterator that points to item found.

            Returns <tt> std::pair<iterator, bool>  </tt> where \p first is an iterator pointing to
            item found or inserted, \p second is true if new item has been added or \p false if the item
            already is in the list.
        */
        template <typename K>
        std::pair<iterator, bool> ensure( const K& key )
        {
            std::pair< node_type *, bool > ret = ensure_at( head(), key );
            return std::make_pair( node_to_iterator( ret.first ), ret.second );
        }

        /// Inserts data of type \ref mapped_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename... Args>
        iterator emplace( Args&&... args )
        {
            return node_to_iterator( emplace_at( head(), std::forward<Args>(args)... ));
        }

        /// Find the key \p key
        /** \anchor cds_nonintrusive_LazyKVList_nogc_find
            The function searches the item with key equal to \p key
            and returns an iterator pointed to item found if the key is found,
            and \ref end() otherwise
        */
        template <typename Q>
        iterator find( Q const& key )
        {
            return node_to_iterator( find_at( head(), key, intrusive_key_comparator() ) );
        }

        /// Finds the key \p val using \p pred predicate for searching (for ordered list only)
        /**
            The function is an analog of \ref cds_nonintrusive_LazyKVList_nogc_find "find(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, bool Sort = c_bSort>
        typename std::enable_if<Sort, iterator>::type find_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return node_to_iterator( find_at( head(), key, typename maker::template less_wrapper<Less>::type() ) );
        }

        /// Finds the key \p val using \p equal predicate for searching (for unordered list only)
        /**
            The function is an analog of \ref cds_nonintrusive_LazyKVList_nogc_find "find(Q const&)"
            but \p equal is used for key comparing.
            \p Equal functor has the interface like \p std::equal_to.
        */
        template <typename Q, typename Equal, bool Sort = c_bSort>
        typename std::enable_if<!Sort, iterator>::type find_with( Q const& key, Equal equal )
        {
            CDS_UNUSED( equal );
            return node_to_iterator( find_at( head(), key, typename maker::template equal_to_wrapper<Equal>::type() ) );
        }

        /// Check if the list is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns list's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact is not mean that the list
            is empty. To check list emptyness use \ref empty() method.
        */
        size_t size() const
        {
            return base_class::size();
        }

        /// Clears the list
        /**
            Post-condition: the list is empty
        */
        void clear()
        {
            base_class::clear();
        }

    protected:
        //@cond
        node_type * insert_node_at( head_type& refHead, node_type * pNode )
        {
            assert( pNode != nullptr );
            scoped_node_ptr p( pNode );
            if ( base_class::insert_at( &refHead, *p ))
                return p.release();

            return nullptr;
        }

        template <typename K>
        node_type * insert_at( head_type& refHead, const K& key )
        {
            return insert_node_at( refHead, alloc_node( key ));
        }

        template <typename K, typename V>
        node_type * insert_at( head_type& refHead, const K& key, const V& val )
        {
            return insert_node_at( refHead, alloc_node( key, val ));
        }

        template <typename K, typename Func>
        node_type * insert_with_at( head_type& refHead, const K& key, Func f )
        {
            scoped_node_ptr pNode( alloc_node( key ));

            if ( base_class::insert_at( &refHead, *pNode )) {
                f( pNode->m_Data );
                return pNode.release();
            }

            return nullptr;
        }


        template <typename K>
        std::pair< node_type *, bool > ensure_at( head_type& refHead, const K& key )
        {
            scoped_node_ptr pNode( alloc_node( key ));
            node_type * pItemFound = nullptr;

            std::pair<bool, bool> ret = base_class::ensure_at( &refHead, *pNode, [&pItemFound](bool, node_type& item, node_type&){ pItemFound = &item; } );
            if ( ret.first && ret.second )
                pNode.release();

            assert( pItemFound != nullptr );
            return std::make_pair( pItemFound, ret.second );
        }

        template <typename... Args>
        node_type * emplace_at( head_type& refHead, Args&&... args )
        {
            return insert_node_at( refHead, alloc_node( std::forward<Args>(args)... ));
        }

        template <typename K, typename Compare>
        node_type * find_at( head_type& refHead, const K& key, Compare cmp )
        {
            return base_class::find_at( &refHead, key, cmp );
        }

        /*
        template <typename K, typenam Compare, typename Func>
        bool find_at( head_type& refHead, K& key, Compare cmp, Func f )
        {
            return base_class::find_at( &refHead, key, cmp, [&f]( node_type& node, K const& ){ f( node.m_Data ); });
        }
        */
        //@endcond
    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_LAZY_KVLIST_NOGC_H
