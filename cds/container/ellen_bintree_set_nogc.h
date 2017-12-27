//
// Created by pasha on 27.12.17.
//

#ifndef CDS_ELLEN_BINTREE_SET_NOGC_H
#define CDS_ELLEN_BINTREE_SET_NOGC_H
#include <cds/container/details/ellen_bintree_base.h>
#include <cds/intrusive/ellen_bintree_nogc.h>
#include <cds/container/details/guarded_ptr_cast.h>
#include "../intrusive/ellen_bintree_nogc.h"

namespace cds { namespace container {

        template <
                typename Key,
                typename T,
#ifdef CDS_DOXYGEN_INVOKED
                class Traits = ellen_bintree::traits
#else
                class Traits
#endif
        >
        class EllenBinTreeSetNogc: public ellen_bintree::details::make_ellen_bintree_set_nogc< gc::nogc,
                Key, T, Traits >::type
        {
            typedef ellen_bintree::details::make_ellen_bintree_set_nogc< gc::nogc, Key, T, Traits > maker;
            typedef typename maker::type base_class;
            //@endcond

        public:
            typedef gc::nogc      gc;         ///< Garbage collector
            typedef Key     key_type;   ///< type of a key to be stored in internal nodes; key is a part of \p value_type
            typedef T       value_type; ///< type of value to be stored in the binary tree
            typedef Traits  traits;    ///< Traits template parameter

#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined key_comparator  ;    ///< key compare functor based on opt::compare and opt::less option setter.
#   else
            typedef typename maker::intrusive_traits::compare   key_comparator;
#   endif
            typedef typename base_class::item_counter           item_counter;  ///< Item counting policy used
            typedef typename base_class::memory_model           memory_model;  ///< Memory ordering. See cds::opt::memory_model option
            typedef typename traits::key_extractor              key_extractor; ///< key extracting functor
            typedef typename traits::back_off                   back_off;      ///< Back-off strategy

            typedef typename traits::allocator                  allocator_type;   ///< Allocator for leaf nodes
            typedef typename base_class::node_allocator         node_allocator;   ///< Internal node allocator
            typedef typename base_class::update_desc_allocator  update_desc_allocator; ///< Update descriptor allocator

        protected:
            //@cond
            typedef typename maker::cxx_leaf_node_allocator cxx_leaf_node_allocator;
            typedef typename base_class::value_type         leaf_node;
            typedef typename base_class::internal_node      internal_node;

            typedef std::unique_ptr< leaf_node, typename maker::leaf_deallocator > scoped_node_ptr;

        public:
            /// Default constructor
            EllenBinTreeSetNogc()
                    : base_class()
            {}

            /// Clears the set
            ~EllenBinTreeSetNogc()
            {}



            /// Inserts new node
            /**
                The function creates a node with copy of \p val value
                and then inserts the node created into the set.

                The type \p Q should contain at least the complete key for the node.
                The object of \ref value_type should be constructible from a value of type \p Q.
                In trivial case, \p Q is equal to \ref value_type.

                Returns \p true if \p val is inserted into the set, \p false otherwise.
            */
            template <typename Q>
            bool insert( Q const& val )
            {
               /* scoped_node_ptr sp( cxx_leaf_node_allocator().New( val ));
                if ( base_class::insert( *sp.get())) {
                    sp.release();
                    return true;
                }*/
                return false;
            }

            /// Inserts new node
            /**
                The function allows to split creating of new item into two part:
                - create item with key only
                - insert new item into the set
                - if inserting is success, calls  \p f functor to initialize value-fields of \p val.

                The functor signature is:
                \code
                    void func( value_type& val );
                \endcode
                where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
                \p val no any other changes could be made on this set's item by concurrent threads.
                The user-defined functor is called only if the inserting is success.
            */
            template <typename Q, typename Func>
            bool insert( Q const& val, Func f )
            {
                /*scoped_node_ptr sp( cxx_leaf_node_allocator().New( val ));
                if ( base_class::insert( *sp.get(), [&f]( leaf_node& v ) { f( v.m_Value ); } )) {
                    sp.release();
                    return true;
                }*/
                return false;
            }

            /// Updates the node
            /**
                The operation performs inserting or changing data with lock-free manner.

                If the item \p val is not found in the set, then \p val is inserted into the set
                iff \p bAllowInsert is \p true.
                Otherwise, the functor \p func is called with item found.
                The functor \p func signature is:
                \code
                    struct my_functor {
                        void operator()( bool bNew, value_type& item, const Q& val );
                    };
                \endcode
                with arguments:
                with arguments:
                - \p bNew - \p true if the item has been inserted, \p false otherwise
                - \p item - item of the set
                - \p val - argument \p key passed into the \p %update() function

                The functor can change non-key fields of the \p item; however, \p func must guarantee
                that during changing no any other modifications could be made on this item by concurrent threads.

                Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
                i.e. the node has been inserted or updated,
                \p second is \p true if new item has been added or \p false if the item with \p key
                already exists.

                @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
            */
            template <typename Q, typename Func>
            std::pair<bool, bool> update( const Q& val, Func func, bool bAllowInsert = true )
            {
                scoped_node_ptr sp( cxx_leaf_node_allocator().New( val ));
                std::pair<bool, bool> bRes = base_class::update( *sp,
                                                                 [&func, &val](bool bNew, leaf_node& node, leaf_node&){ func( bNew, node.m_Value, val ); },
                                                                 bAllowInsert );
                if ( bRes.first && bRes.second )
                    sp.release();
                return bRes;
            }
            //@cond
            template <typename Q, typename Func>
            CDS_DEPRECATED("ensure() is deprecated, use update()")
            std::pair<bool, bool> ensure( const Q& val, Func func )
            {
                return update( val, func, true );
            }
            //@endcond

            /// Inserts data of type \p value_type created in-place from \p args
            /**
                Returns \p true if inserting successful, \p false otherwise.
            */
            template <typename... Args>
            bool emplace( Args&&... args )
            {
                scoped_node_ptr sp( cxx_leaf_node_allocator().MoveNew( std::forward<Args>(args)... ));
                if ( base_class::insert( *sp.get())) {
                    sp.release();
                    return true;
                }
                return false;
            }

            /// Find the key \p key
            /**
                @anchor cds_nonintrusive_EllenBinTreeSet_find_func

                The function searches the item with key equal to \p key and calls the functor \p f for item found.
                The interface of \p Func functor is:
                \code
                struct functor {
                    void operator()( value_type& item, Q& key );
                };
                \endcode
                where \p item is the item found, \p key is the <tt>find</tt> function argument.

                The functor may change non-key fields of \p item. Note that the functor is only guarantee
                that \p item cannot be disposed during functor is executing.
                The functor does not serialize simultaneous access to the set's \p item. If such access is
                possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

                The \p key argument is non-const since it can be used as \p f functor destination i.e., the functor
                can modify both arguments.

                Note the hash functor specified for class \p Traits template parameter
                should accept a parameter of type \p Q that may be not the same as \p value_type.

                The function returns \p true if \p key is found, \p false otherwise.
            */
            template <typename Q, typename Func>
            bool find( Q& key, Func f )
            {
                return base_class::find( key, [&f]( leaf_node& node, Q& v ) { f( node.m_Value, v ); });
            }
            //@cond
            template <typename Q, typename Func>
            bool find( Q const& key, Func f )
            {
                return base_class::find( key, [&f]( leaf_node& node, Q const& v ) { f( node.m_Value, v ); } );
            }
            //@endcond

            /// Finds the key \p key using \p pred predicate for searching
            /**
                The function is an analog of \ref cds_nonintrusive_EllenBinTreeSet_find_func "find(Q&, Func)"
                but \p pred is used for key comparing.
                \p Less functor has the interface like \p std::less.
                \p Less must imply the same element order as the comparator used for building the set.
            */
            template <typename Q, typename Less, typename Func>
            bool find_with( Q& key, Less pred, Func f )
            {
                CDS_UNUSED( pred );
                return base_class::find_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >(),
                                              [&f]( leaf_node& node, Q& v ) { f( node.m_Value, v ); } );
            }
            //@cond
            template <typename Q, typename Less, typename Func>
            bool find_with( Q const& key, Less pred, Func f )
            {
                CDS_UNUSED( pred );
                return base_class::find_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >(),
                                              [&f]( leaf_node& node, Q const& v ) { f( node.m_Value, v ); } );
            }
            //@endcond

            /// Checks whether the set contains \p key
            /**
                The function searches the item with key equal to \p key
                and returns \p true if it is found, and \p false otherwise.
            */
            template <typename Q>
            bool contains( Q const & key )
            {
                return base_class::contains( key );
            }
            //@cond
            template <typename Q>
            CDS_DEPRECATED("deprecated, use contains()")
            bool find( Q const & key )
            {
                return contains( key );
            }
            //@endcond

            /// Checks whether the set contains \p key using \p pred predicate for searching
            /**
                The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
                \p Less functor has the interface like \p std::less.
                \p Less must imply the same element order as the comparator used for building the set.
            */
            template <typename Q, typename Less>
            bool contains( Q const& key, Less pred )
            {
                CDS_UNUSED( pred );
                return base_class::contains( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >());
            }
            //@cond
            template <typename Q, typename Less>
            CDS_DEPRECATED("deprecated, use contains()")
            bool find_with( Q const& key, Less pred )
            {
                return contains( key, pred );
            }
            //@endcond

            /// Checks if the set is empty
            bool empty() const
            {
                return base_class::empty();
            }

            /// Returns item count in the set
            /**
                Only leaf nodes containing user data are counted.

                The value returned depends on item counter type provided by \p Traits template parameter.
                If it is \p atomicity::empty_item_counter this function always returns 0.

                The function is not suitable for checking the tree emptiness, use \p empty()
                member function for this purpose.
            */
            size_t size() const
            {
                return base_class::size();
            }
        };
    }
}



#endif //CDS_ELLEN_BINTREE_SET_NOGC_H
