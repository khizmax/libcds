// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_STRIPED_SET_ADAPTER_H
#define CDSLIB_INTRUSIVE_STRIPED_SET_ADAPTER_H

#include <cds/opt/options.h>
#include <cds/intrusive/striped_set/resizing_policy.h>
#include <cds/opt/hash.h>
#include <cds/opt/compare.h>    // cds::opt::details::make_comparator - for some adapt specializations

namespace cds { namespace intrusive {

    /// StripedSet related definitions
    namespace striped_set {
        /// Default adapter for intrusive striped/refinable hash set
        /**
            By default, the metafunction does not make any transformation for container type \p Container.
            \p Container should provide interface suitable for the hash set.

            The \p Options template argument contains option pack
            that will be passed to \p cds::intrusive::StripedSet.

        <b>Bucket interface</b>

            The result of metafunction is a container (a bucket) that should support the following interface:

            Public typedefs that the bucket should provide:
                - \p value_type - the type of the item in the bucket
                - \p iterator - bucket's item iterator
                - \p const_iterator - bucket's item constant iterator
                - \p default_resizing_policy - default resizing policy preferable for the container.
                    By default, the library defines cds::container::striped_set::load_factor_resizing<4> for sequential containers like
                    boost::intrusive::list,  and cds::container::striped_set::no_resizing for ordered container like boost::intrusive::set.

            <b>Insert value \p val of type \p Q</b>
            \code template <typename Func> bool insert( value_type& val, Func f ) ; \endcode
                Inserts \p val into the container and, if inserting is successful, calls functor \p f
                with \p val.

                The functor signature is:
                \code
                struct functor {
                    void operator()( value_type& item );
                };
                \endcode
                where \p item is the item inserted.

                The user-defined functor \p f is called only if the inserting is success.
                <hr>

            <b>Updates the item in the container</b>
            \code template <typename Func> std::pair<bool, bool> update( value_type& val, Func f, bool bAllowInsert = true ) \endcode
                The operation performs inserting or changing data.

                If the \p val key not found in the container, then \p val is inserted iff \p bAllowInsert is \p true.
                Otherwise, the functor \p f is called with the item found.

                The \p Func functor has the following interface:
                \code
                    void func( bool bNew, value_type& item, value_type& val );
                \endcode
                or like a functor:
                \code
                    struct functor {
                        void operator()( bool bNew, value_type& item, value_type& val );
                    };
                \endcode

                where arguments are:
                - \p bNew - \p true if the item has been inserted, \p false otherwise
                - \p item - container's item
                - \p val - argument \p val passed into the \p update() function

                If \p val has been inserted (i.e. <tt>bNew == true</tt>) then \p item and \p val
                are the same element: <tt>&item == &val</tt>. Otherwise, they are different.

                The functor can change non-key fields of the \p item.

                Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
                \p second is true if new item has been added or \p false if the item with \p val key
                already exists.
                <hr>

            <b>Unlink an item</b>
            \code bool unlink( value_type& val ) \endcode
                Unlink \p val from the container if \p val belongs to it.
                <hr>

            <b>Erase \p key</b>
            \code template <typename Q, typename Func> bool erase( Q const& key, Func f ) \endcode
                The function searches an item with key \p key, calls \p f functor
                and erases the item. If \p key is not found, the functor is not called.

                The functor \p Func interface is:
                \code
                struct functor {
                    void operator()(value_type& val);
                };
                \endcode

                The type \p Q can differ from \ref value_type of items storing in the container.
                Therefore, the \p value_type should be comparable with type \p Q.

                Return \p true if key is found and deleted, \p false otherwise
                <hr>


            <b>Find the key \p val </b>
            \code
            template <typename Q, typename Func> bool find( Q& val, Func f )
            template <typename Q, typename Compare, typename Func> bool find( Q& val, Compare cmp, Func f )
            \endcode
                The function searches the item with key equal to \p val and calls the functor \p f for item found.
                The interface of \p Func functor is:
                \code
                struct functor {
                    void operator()( value_type& item, Q& val );
                };
                \endcode
                where \p item is the item found, \p val is the <tt>find</tt> function argument.

                The functor can change non-key fields of \p item.
                The \p val argument may be non-const since it can be used as \p f functor destination i.e., the functor
                can modify both arguments.

                The type \p Q can differ from \ref value_type of items storing in the container.
                Therefore, the \p value_type should be comparable with type \p Q.

                The first form uses default \p compare function used for key ordering.
                The second form allows to point specific \p Compare functor \p cmp
                that can compare \p value_typwe and \p Q type. The interface of \p Compare is the same as \p std::less.

                The function returns \p true if \p val is found, \p false otherwise.
                <hr>

            <b>Clears the container</b>
            \code
            void clear()
            template <typename Disposer> void clear( Disposer disposer )
            \endcode
            Second form calls \p disposer for each item in the container before clearing.
            <hr>

            <b>Get size of bucket</b>
            \code size_t size() const \endcode
            This function may be required by some resizing policy
            <hr>

            <b>Iterators</b>
            \code
            iterator begin();
            const_iterator begin() const;
            iterator end();
            const_iterator end() const;
            \endcode
            <hr>

            <b>Move item when resizing</b>
            \code void move_item( adapted_container& from, iterator it ) \endcode
                This helper function is invented for the set resizing when the item
                pointed by \p it iterator is copied from old bucket \p from to a new bucket
                pointed by \p this.
            <hr>

        */
        template < typename Container, typename... Options >
        class adapt
        {
        public:
            typedef Container   type            ;   ///< adapted container type
            typedef typename type::value_type value_type  ;   ///< value type stored in the container
        };

        //@cond
        struct adapted_sequential_container
        {
            typedef striped_set::load_factor_resizing<4>   default_resizing_policy;
        };

        struct adapted_container
        {
            typedef striped_set::no_resizing   default_resizing_policy;
        };
        //@endcond

        //@cond
        namespace details {
            template <typename Set>
            class boost_intrusive_set_adapter: public cds::intrusive::striped_set::adapted_container
            {
            public:
                typedef Set container_type;

                typedef typename container_type::value_type     value_type      ;   ///< value type stored in the container
                typedef typename container_type::iterator       iterator        ;   ///< container iterator
                typedef typename container_type::const_iterator const_iterator  ;   ///< container const iterator

                typedef typename container_type::key_compare  key_comparator;

            private:
                container_type  m_Set;

            public:
                boost_intrusive_set_adapter()
                {}

                container_type& base_container()
                {
                    return m_Set;
                }

                template <typename Func>
                bool insert( value_type& val, Func f )
                {
                    std::pair<iterator, bool> res = m_Set.insert( val );
                    if ( res.second )
                        f( val );
                    return res.second;
                }

                template <typename Func>
                std::pair<bool, bool> update( value_type& val, Func f, bool bAllowInsert )
                {
                    if ( bAllowInsert ) {
                        std::pair<iterator, bool> res = m_Set.insert( val );
                        f( res.second, *res.first, val );
                        return std::make_pair( true, res.second );
                    }
                    else {
                        auto it = m_Set.find( val, key_comparator());
                        if ( it == m_Set.end())
                            return std::make_pair( false, false );
                        f( false, *it, val );
                        return std::make_pair( true, false );
                    }
                }

                bool unlink( value_type& val )
                {
                    iterator it = m_Set.find( val, key_comparator());
                    if ( it == m_Set.end() || &(*it) != &val )
                        return false;
                    m_Set.erase( it );
                    return true;
                }

                template <typename Q, typename Func>
                value_type * erase( Q const& key, Func f )
                {
                    iterator it = m_Set.find( key, key_comparator());
                    if (it == m_Set.end())
                        return nullptr;
                    value_type& val = *it;
                    f( val );
                    m_Set.erase( it );
                    return &val;
                }

                template <typename Q, typename Less, typename Func>
                value_type * erase( Q const& key, Less pred, Func f )
                {
                    iterator it = m_Set.find( key, pred );
                    if (it == m_Set.end())
                        return nullptr;
                    value_type& val = *it;
                    f( val );
                    m_Set.erase( it );
                    return &val;
                }

                template <typename Q, typename Func>
                bool find( Q const& key, Func f )
                {
                    return find( key, key_comparator(), f );
                }

                template <typename Q, typename Compare, typename Func>
                bool find( Q const& key, Compare cmp, Func f )
                {
                    iterator it = m_Set.find( key, cmp );
                    if ( it == m_Set.end())
                        return false;
                    f( *it, key );
                    return true;
                }

                void clear()
                {
                    m_Set.clear();
                }

                template <typename Disposer>
                void clear( Disposer disposer )
                {
                    m_Set.clear_and_dispose( disposer );
                }

                iterator begin()                { return m_Set.begin(); }
                const_iterator begin() const    { return m_Set.begin(); }
                iterator end()                  { return m_Set.end(); }
                const_iterator end() const      { return m_Set.end(); }

                size_t size() const
                {
                    return (size_t) m_Set.size();
                }

                void move_item( boost_intrusive_set_adapter& from, iterator itWhat )
                {
                    value_type& val = *itWhat;
                    from.base_container().erase( itWhat );
                    insert( val, []( value_type& ) {} );
                }
            };
        }   // namespace details
        //@endcond

    } // namespace striped_set
}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_STRIPED_SET_ADAPTER_H
