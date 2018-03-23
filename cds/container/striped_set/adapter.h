// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_STRIPED_SET_ADAPTER_H
#define CDSLIB_CONTAINER_STRIPED_SET_ADAPTER_H

#include <cds/intrusive/striped_set/adapter.h>
#include <cds/intrusive/striped_set/striping_policy.h>

namespace cds { namespace container {
    /// Striped hash set related definitions
    namespace striped_set {

        //@cond
        struct copy_item    ;   // copy_item_policy tag
        template <typename Container>
        struct copy_item_policy;

        struct swap_item    ;   // swap_item_policy tag
        template <typename Container>
        struct swap_item_policy;

        struct move_item    ;   // move_item_policy tag
        template <typename Container>
        struct move_item_policy;
        //@endcond

#ifdef CDS_DOXYGEN_INVOKED
        /// Default adapter for hash set
        /**
            By default, the metafunction does not make any transformation for container type \p Container.
            \p Container should provide interface suitable for the hash set.

            The \p Options template argument contains a list of options
            that has been passed to cds::container::StripedSet.

        <b>Bucket interface</b>

            The result of metafunction is a container (a bucket) that should support the following interface:

            Public typedefs that the bucket should provide:
                - \p value_type - the type of the item in the bucket
                - \p iterator - bucket's item iterator
                - \p const_iterator - bucket's item constant iterator
                - \p default_resizing_policy - defalt resizing policy preferable for the container.
                    By default, the library defines striped_set::load_factor_resizing<4> for sequential containers like
                    std::list, std::vector, and striped_set::no_resizing for ordered container like std::set,
                    std::unordered_set.

            <b>Insert value \p val of type \p Q</b>
            \code template <typename Q, typename Func> bool insert( const Q& val, Func f ) ; \endcode
                The function allows to split creating of new item into two part:
                - create item with key only from \p val
                - try to insert new item into the container
                - if inserting is success, calls \p f functor to initialize value-field of the new item.

                The functor signature is:
                \code
                    void func( value_type& item );
                \endcode
                where \p item is the item inserted.

                The type \p Q can differ from \ref value_type of items storing in the container.
                Therefore, the \p value_type should be comparable with type \p Q and constructible from type \p Q,

                The user-defined functor is called only if the inserting is success.
                <hr>

            <b>Inserts data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt></b>
            \code template <typename... Args> bool emplace( Args&&... args ) ; \endcode
                Returns \p true if inserting successful, \p false otherwise.

                This function should be available only for compiler that supports
                variadic template and move semantics
            <hr>

            <b>Updates \p item</b>
            \code template <typename Q, typename Func> std::pair<bool, bool> update( const Q& val, Func func, bool bAllowInsert ) \endcode
                The operation performs inserting or changing data.

                If the \p val key not found in the container, then the new item created from \p val
                is inserted iff \p bAllowInsert is \p true. Otherwise, the functor \p func is called with the item found.
                The \p Func functor has interface:
                \code
                    void func( bool bNew, value_type& item, const Q& val );
                \endcode
                or like a functor:
                \code
                    struct my_functor {
                        void operator()( bool bNew, value_type& item, const Q& val );
                    };
                \endcode

                where arguments are:
                - \p bNew - \p true if the item has been inserted, \p false otherwise
                - \p item - container's item
                - \p val - argument \p val passed into the \p update() function

                The functor can change non-key fields of the \p item.

                The type \p Q can differ from \ref value_type of items storing in the container.
                Therefore, the \p value_type should be comparable with type \p Q and constructible from type \p Q,

                Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
                \p second is true if new item has been added or \p false if the item with \p val key
                already exists.
                <hr>


            <b>Delete \p key</b>
            \code template <typename Q, typename Func> bool erase( const Q& key, Func f ) \endcode
                The function searches an item with key \p key, calls \p f functor
                and deletes the item. If \p key is not found, the functor is not called.

                The functor \p Func interface is:
                \code
                struct extractor {
                    void operator()(value_type const& val);
                };
                \endcode

                The type \p Q can differ from \ref value_type of items storing in the container.
                Therefore, the \p value_type should be comparable with type \p Q.

                Return \p true if key is found and deleted, \p false otherwise
                <hr>


            <b>Find the key \p val </b>
            \code template <typename Q, typename Func> bool find( Q& val, Func f ) \endcode
                The function searches the item with key equal to \p val and calls the functor \p f for item found.
                The interface of \p Func functor is:
                \code
                struct functor {
                    void operator()( value_type& item, Q& val );
                };
                \endcode
                where \p item is the item found, \p val is the <tt>find</tt> function argument.

                The functor can change non-key fields of \p item.
                The \p val argument is non-const since it can be used as \p f functor destination i.e., the functor
                can modify both arguments.

                The type \p Q can differ from \ref value_type of items storing in the container.
                Therefore, the \p value_type should be comparable with type \p Q.

                The function returns \p true if \p val is found, \p false otherwise.
                <hr>

            <b>Clears the container</b>
            \code void clear() \endcode
            <hr>

            <b>Get size of bucket</b>
            \code size_t size() const \endcode
            This function can be required by some resizing policy
            <hr>

            <b>Move item when resizing</b>
            \code void move_item( adapted_container& from, iterator it ) \endcode
            This helper function is invented for the set resizing when the item
            pointed by \p it iterator is copied from an old bucket \p from to a new bucket
            pointed by \p this.
            <hr>

        */
        template < typename Container, typename... Options>
        class adapt
        {
        public:
            typedef Container   type            ;   ///< adapted container type
            typedef typename type::value_type value_type  ;   ///< value type stored in the container
        };
#else   // CDS_DOXYGEN_INVOKED
        using cds::intrusive::striped_set::adapt;
#endif

        //@cond
        using cds::intrusive::striped_set::adapted_sequential_container;
        using cds::intrusive::striped_set::adapted_container;
        //@endcond

        ///@copydoc cds::intrusive::striped_set::load_factor_resizing
        template <size_t LoadFactor>
        using load_factor_resizing = cds::intrusive::striped_set::load_factor_resizing<LoadFactor>;

        ///@copydoc cds::intrusive::striped_set::rational_load_factor_resizing
        template <size_t Numerator, size_t Denominator = 1>
        using rational_load_factor_resizing = cds::intrusive::striped_set::rational_load_factor_resizing<Numerator, Denominator>;

        ///@copydoc cds::intrusive::striped_set::single_bucket_size_threshold
        template <size_t Threshold>
        using single_bucket_size_threshold = cds::intrusive::striped_set::single_bucket_size_threshold<Threshold>;

        ///@copydoc cds::intrusive::striped_set::no_resizing
        typedef cds::intrusive::striped_set::no_resizing no_resizing;

        ///@copydoc cds::intrusive::striped_set::striping
        template <class Lock = std::mutex, class Alloc = CDS_DEFAULT_ALLOCATOR >
        using striping = cds::intrusive::striped_set::striping<Lock, Alloc>;

        ///@copydoc cds::intrusive::striped_set::refinable
        template <
            class RecursiveLock = std::recursive_mutex,
            typename BackOff = cds::backoff::yield,
            class Alloc = CDS_DEFAULT_ALLOCATOR
        >
        using refinable = cds::intrusive::striped_set::refinable<RecursiveLock, BackOff, Alloc >;

        //@cond
        namespace details {

            template <class Set>
            struct boost_set_copy_policies
            {
                struct copy_item_policy
                {
                    typedef Set set_type;
                    typedef typename set_type::iterator iterator;

                    void operator()( set_type& set, iterator itWhat )
                    {
                        set.insert( *itWhat );
                    }
                };

                typedef copy_item_policy swap_item_policy;

              struct move_item_policy
                {
                    typedef Set set_type;
                    typedef typename set_type::iterator iterator;

                    void operator()( set_type& set, iterator itWhat )
                    {
                        set.insert( std::move( *itWhat ));
                    }
                };
            };

            template <class Set, typename... Options>
            class boost_set_adapter: public striped_set::adapted_container
            {
            public:
                typedef Set container_type;

                typedef typename container_type::value_type     value_type      ;   ///< value type stored in the container
                typedef typename container_type::iterator       iterator        ;   ///< container iterator
                typedef typename container_type::const_iterator const_iterator  ;   ///< container const iterator

                static bool const has_find_with = false;
                static bool const has_erase_with = false;

            private:
                typedef typename cds::opt::select<
                    typename cds::opt::value<
                        typename cds::opt::find_option<
                            cds::opt::copy_policy< cds::container::striped_set::move_item >
                            , Options...
                        >::type
                    >::copy_policy
                    , cds::container::striped_set::copy_item, copy_item_policy<container_type>
                    , cds::container::striped_set::swap_item, swap_item_policy<container_type>
                    , cds::container::striped_set::move_item, move_item_policy<container_type>
                >::type copy_item;

            private:
                container_type  m_Set;

            public:
                boost_set_adapter()
                {}

                container_type& base_container()
                {
                    return m_Set;
                }

                template <typename Q, typename Func>
                bool insert( const Q& val, Func f )
                {
                    std::pair<iterator, bool> res = m_Set.insert( value_type(val));
                    if ( res.second )
                        f( const_cast<value_type&>(*res.first));
                    return res.second;
                }

                template <typename... Args>
                bool emplace( Args&&... args )
                {
                    std::pair<iterator, bool> res = m_Set.emplace( std::forward<Args>(args)... );
                    return res.second;
                }

                template <typename Q, typename Func>
                std::pair<bool, bool> update( const Q& val, Func func, bool bAllowInsert )
                {
                    if ( bAllowInsert ) {
                        std::pair<iterator, bool> res = m_Set.insert( value_type(val));
                        func( res.second, const_cast<value_type&>(*res.first), val );
                        return std::make_pair( true, res.second );
                    }
                    else {
                        auto it = m_Set.find( value_type( val ));
                        if ( it == m_Set.end())
                            return std::make_pair( false, false );
                        func( false, const_cast<value_type&>(*it), val );
                        return std::make_pair( true, false );
                    }
                }

                template <typename Q, typename Func>
                bool erase( const Q& key, Func f )
                {
                    const_iterator it = m_Set.find( value_type(key));
                    if ( it == m_Set.end())
                        return false;
                    f( const_cast<value_type&>(*it));
                    m_Set.erase( it );
                    return true;
                }

                template <typename Q, typename Func>
                bool find( Q& val, Func f )
                {
                    iterator it = m_Set.find( value_type(val));
                    if ( it == m_Set.end())
                        return false;
                    f( const_cast<value_type&>(*it), val );
                    return true;
                }

                void clear()
                {
                    m_Set.clear();
                }

                iterator begin()                { return m_Set.begin(); }
                const_iterator begin() const    { return m_Set.begin(); }
                iterator end()                  { return m_Set.end(); }
                const_iterator end() const      { return m_Set.end(); }

                void move_item( adapted_container& /*from*/, iterator itWhat )
                {
                    assert( m_Set.find( *itWhat ) == m_Set.end());
                    copy_item()( m_Set, itWhat );
                }

                size_t size() const
                {
                    return m_Set.size();
                }
            };

            template <class Map>
            struct boost_map_copy_policies {
                struct copy_item_policy {
                    typedef Map map_type;
                    typedef typename map_type::value_type pair_type;
                    typedef typename map_type::iterator    iterator;

                    void operator()( map_type& map, iterator itWhat )
                    {
                        map.insert( *itWhat );
                    }
                };

                struct swap_item_policy {
                    typedef Map map_type;
                    typedef typename map_type::value_type pair_type;
                    typedef typename map_type::iterator    iterator;

                    void operator()( map_type& map, iterator itWhat )
                    {
                        std::pair< iterator, bool > ret = map.insert( pair_type( itWhat->first, typename pair_type::second_type()));
                        assert( ret.second )    ;   // successful insertion
                        std::swap( ret.first->second, itWhat->second );
                    }
                };

                struct move_item_policy {
                    typedef Map map_type;
                    typedef typename map_type::value_type pair_type;
                    typedef typename map_type::iterator    iterator;

                    void operator()( map_type& map, iterator itWhat  )
                    {
                        map.insert( std::move( *itWhat ));
                    }
                };
            };

            template <class Map, typename... Options>
            class boost_map_adapter: public striped_set::adapted_container
            {
            public:
                typedef Map container_type;

                typedef typename container_type::value_type value_type  ;   ///< value type stored in the container
                typedef typename container_type::key_type   key_type;
                typedef typename container_type::mapped_type    mapped_type;
                typedef typename container_type::iterator       iterator        ;   ///< container iterator
                typedef typename container_type::const_iterator const_iterator  ;   ///< container const iterator

                static bool const has_find_with = false;
                static bool const has_erase_with = false;

            private:
                typedef typename cds::opt::select<
                    typename cds::opt::value<
                    typename cds::opt::find_option<
                        cds::opt::copy_policy< cds::container::striped_set::move_item >
                        , Options...
                    >::type
                    >::copy_policy
                    , cds::container::striped_set::copy_item, copy_item_policy<container_type>
                    , cds::container::striped_set::swap_item, swap_item_policy<container_type>
                    , cds::container::striped_set::move_item, move_item_policy<container_type>
                >::type copy_item;

            private:
                container_type  m_Map;

            public:
                template <typename Q, typename Func>
                bool insert( const Q& key, Func f )
                {
                    std::pair<iterator, bool> res = m_Map.insert( value_type( key_type( key ), mapped_type()));
                    if ( res.second )
                        f( *res.first );
                    return res.second;
                }

                template <typename Q, typename... Args>
                bool emplace( Q&& key, Args&&... args )
                {
                    std::pair<iterator, bool> res = m_Map.emplace( key_type( std::forward<Q>( key )), mapped_type( std::forward<Args>( args )...));
                    return res.second;
                }

                template <typename Q, typename Func>
                std::pair<bool, bool> update( const Q& key, Func func, bool bAllowInsert )
                {
                    if ( bAllowInsert ) {
                        std::pair<iterator, bool> res = m_Map.insert( value_type( key_type( key ), mapped_type()));
                        func( res.second, *res.first );
                        return std::make_pair( true, res.second );
                    }
                    else {
                        auto it = m_Map.find( key_type( key ));
                        if ( it == end())
                            return std::make_pair( false, false );
                        func( false, *it );
                        return std::make_pair( true, false );
                    }
                }

                template <typename Q, typename Func>
                bool erase( const Q& key, Func f )
                {
                    iterator it = m_Map.find( key_type( key ));
                    if ( it == m_Map.end())
                        return false;
                    f( *it );
                    m_Map.erase( it );
                    return true;
                }

                template <typename Q, typename Func>
                bool find( Q& val, Func f )
                {
                    iterator it = m_Map.find( key_type( val ));
                    if ( it == m_Map.end())
                        return false;
                    f( *it, val );
                    return true;
                }

                void clear()
                {
                    m_Map.clear();
                }

                iterator begin()                { return m_Map.begin(); }
                const_iterator begin() const    { return m_Map.begin(); }
                iterator end()                  { return m_Map.end(); }
                const_iterator end() const      { return m_Map.end(); }

                void move_item( adapted_container& /*from*/, iterator itWhat )
                {
                    assert( m_Map.find( itWhat->first ) == m_Map.end());
                    copy_item()( m_Map, itWhat );
                }

                size_t size() const
                {
                    return m_Map.size();
                }
            };

        } // namespace details
        //@endcond

    }   // namespace striped_set
}} // namespace cds::container


#endif // #ifndef CDSLIB_CONTAINER_STRIPED_SET_ADAPTER_H
