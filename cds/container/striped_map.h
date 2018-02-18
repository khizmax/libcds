// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_STRIPED_MAP_H
#define CDSLIB_CONTAINER_STRIPED_MAP_H

#include <type_traits>
#include <cds/container/striped_set.h>
#include <cds/container/striped_set/adapter.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace container {

    //@cond
    namespace details {
        template <class Container, typename... Options>
        class make_striped_map
        {
            typedef StripedSet< Container, Options...>    billet;
            typedef typename billet::options                billet_options;
            typedef typename billet_options::hash           billet_hash;

            typedef typename Container::value_type          pair_type;
            typedef typename pair_type::first_type          key_type;

            struct options: public billet_options {
                struct hash: public billet_hash {
                    size_t operator()( pair_type const& v ) const
                    {
                        return billet_hash::operator()( v.first );
                    }

                    template <typename Q>
                    size_t operator()( Q const& v ) const
                    {
                        return billet_hash::operator()( v );
                    }
                };
            };

        public:
            typedef StripedSet< Container, cds::opt::type_traits< options > >   type ;  ///< metafunction result
        };
    }
    //@endcond

    /// Striped hash map
    /** @ingroup cds_nonintrusive_map

        Source
            - [2008] Maurice Herlihy, Nir Shavit "The Art of Multiprocessor Programming"

        Lock striping is very simple technique.
        The map consists of the bucket table and the array of locks.
        Initially, the capacity of lock array and bucket table is the same.
        When the map is resized, bucket table capacity will be doubled but lock array will not.
        The lock \p i protects each bucket \p j, where <tt> j = i mod L </tt>,
        where \p L - the size of lock array.

        Template arguments:
            - \p Container - the container class that is used as bucket entry. The \p Container class should support
                an uniform interface described below.
            - \p Options - options

        The \p %StripedMap class does not exactly specify the type of container that should be used as a \p Container bucket.
        Instead, the class supports different container type for the bucket, for exampe, \p std::list, \p std::map and others.

        Remember that \p %StripedMap class algorithm ensures sequential blocking access to its bucket through the mutex type you specify
        among \p Options template arguments.

        The \p Options are:
            - \p cds::opt::mutex_policy - concurrent access policy.
                Available policies: \p striped_set::striping, \p striped_set::refinable.
                Default is \p %striped_set::striping.
            - \p cds::opt::hash - hash functor. Default option value see <tt>opt::v::hash_selector<opt::none> </tt>
                which selects default hash functor for your compiler.
            - \p cds::opt::compare - key comparison functor. No default functor is provided.
                If the option is not specified, the \p %opt::less is used.
            - \p cds::opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
            - \p cds::opt::item_counter - item counter type. Default is \p atomicity::item_counter since some operation on the counter is performed
                without locks. Note that item counting is an essential part of the map algorithm, so dummy counter
                like as \p atomicity::empty_item_counter is not suitable.
            - \p cds::opt::allocator - the allocator type using for memory allocation of bucket table and lock array. Default is \ref CDS_DEFAULT_ALLOCATOR.
            - \p cds::opt::resizing_policy - the resizing policy that is a functor that decides when to resize the hash map.
                Default option value depends on bucket container type:
                    for sequential containers like \p std::list, \p std::vector the resizing policy is <tt>striped_set::load_factor_resizing<4> </tt>;
                    for other type of containers like \p std::map, \p std::unordered_map the resizing policy is \p striped_set::no_resizing.
                See \ref cds_striped_resizing_policy "available resizing policy".
                Note that the choose of resizing policy depends of \p Container type:
                for sequential containers like \p std::list, \p std::vector and so on, right choosing of the policy can
                significantly improve performance.
                For other, non-sequential types of \p Container (like a \p std::map)
                the resizing policy is not so important.
            - \p cds::opt::copy_policy - the copy policy which is used to copy items from the old map to the new one when resizing.
                The policy can be optionally used in adapted bucket container for performance reasons of resizing.
                The detail of copy algorithm depends on type of bucket container and explains below.

            \p %opt::compare or \p %opt::less options are used only in some \p Container class for searching an item.
            \p %opt::compare option has the highest priority: if \p %opt::compare is specified, \p %opt::less is not used.

        You can pass other option that would be passed to <tt>adapt</tt> metafunction, see below.

        <b>Internal details</b>

            The \p %StripedMap class cannot utilize the \p Container container specified directly, but only its adapted variant which
            supports an unified interface. Internally, the adaptation is made via \p striped_set::adapt metafunction that wraps bucket container
            and provides the unified bucket interface suitable for \p %StripedMap. Such adaptation is completely transparent for you -
            you don't need to call \p adapt metafunction directly, \p %StripedMap class's internal machinery itself invokes appropriate
            \p adapt metafunction to adjust your \p Container container class to \p %StripedMap bucket's internal interface.
            All you need is to include a right header before <tt>striped_hash_map.h</tt>.

            By default, <tt>striped_set::adapt<AnyContainer, Options...> </tt> metafunction does not make any wrapping to \p AnyContainer,
            so, the result <tt>striped_set::adapt<AnyContainer, Options...>::type </tt> is the same as \p AnyContainer.
            However, there are a lot of specializations of \p adapt for well-known containers, see table below.
            Any of this specialization wraps corresponding container making it suitable for the map's bucket.
            Remember, you should include the proper header file for \p adapt <b>before</b> <tt>striped_map.h</tt>.
            <table>
                <tr>
                    <th>Container</th>
                    <th>.h-file for \p adapt</th>
                    <th>Example</th>
                    <th>Notes</th>
                </tr>
                <tr>
                    <td> \p std::list</td>
                    <td><tt><cds/container/striped_map/std_list.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_map/std_list.h>
                        #include <cds/container/striped_hash_map.h>
                        typedef cds::container::StripedMap<
                            std::list< std::pair< const Key, V > >,
                            cds::opt::less< std::less<Key> >
                        > striped_map;
                    \endcode
                    </td>
                    <td>
                        The type of values stored in the \p std::list must be <tt> std::pair< const Key, V > </tt>, where \p Key - key type,  and \p V - value type
                        The list is ordered by key \p Key.
                        Template argument pack \p Options <b>must</b> contain \p cds::opt::less or \p cds::opt::compare for type \p Key stored in the list.
                    </td>
                </tr>
                <tr>
                    <td> \p std::map</td>
                    <td><tt><cds/container/striped_map/std_map.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_map/std_map.h>
                        #include <cds/container/striped_hash_map.h>
                        typedef cds::container::StripedMap<
                            std::map< Key, T, std::less<Key> >
                        > striped_map;
                    \endcode
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td> \p std::unordered_map</td>
                    <td><tt><cds/container/striped_map/std_hash_map.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_map/std_hash_map.h>
                        #include <cds/container/striped_hash_map.h>
                        typedef cds::container::StripedMap<
                            std::unordered_map<
                                Key, T,
                                std::hash<Key>,
                                std::equal_to<Key>
                            >
                        > striped_map;
                    \endcode
                    </td>
                    <td>
                        You should provide two different hash function \p h1 and \p h2 - one for std::unordered_map and other for \p %StripedMap.
                        For the best result, \p h1 and \p h2 must be orthogonal i.e. <tt> h1(X) != h2(X) </tt> for any value \p X of type \p Key.
                    </td>
                </tr>
                <tr>
                    <td> \p boost::container::slist</td>
                    <td><tt><cds/container/striped_map/boost_slist.h></tt></td>
                    <td>\code
                        #include <cds/container/hash_smap/boost_slist.h>
                        #include <cds/container/striped_hash_map.h>
                        typedef cds::container::StripedMap<
                            boost::container::slist< std::pair< const Key, T > >
                        > striped_map;
                    \endcode
                    </td>
                    <td>
                        The type of values stored in the \p boost::container::slist must be <tt> std::pair< const Key, T > </tt>,
                        where \p Key - key type,  and \p T - value type. The list is ordered.
                        \p Options <b>must</b> contain \p cds::opt::less or \p cds::opt::compare.
                    </td>
                </tr>
                <tr>
                    <td> \p boost::container::list</td>
                    <td><tt><cds/container/striped_map/boost_list.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_map/boost_list.h>
                        #include <cds/container/striped_hash_map.h>
                        typedef cds::container::StripedMap<
                            boost::container::list< std::pair< const Key, T > >
                        > striped_map;
                    \endcode
                    </td>
                    <td>
                        The type of values stored in the \p boost::container::list must be <tt> std::pair< const Key, T > </tt>,
                        where \p Key - key type,  and \p T - value type. The list is ordered.
                        \p Options <b>must</b> contain \p cds::opt::less or \p cds::opt::compare.
                    </td>
                </tr>
                <tr>
                    <td> \p boost::container::map</td>
                    <td><tt><cds/container/striped_map/boost_map.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_map/boost_map.h>
                        #include <cds/container/striped_hash_map.h>
                        typedef cds::container::StripedMap<
                            boost::container::map< Key, T, std::less<Key> >
                        > striped_map;
                    \endcode
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td> \p boost::container::flat_map</td>
                    <td><tt><cds/container/striped_map/boost_flat_map.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_map/boost_flat_map.h>
                        #include <cds/container/striped_hash_map.h>
                        typedef cds::container::StripedMap<
                            boost::container::flat_map< Key, T,
                                std::less< std::less<Key> >
                            >
                        > striped_map;
                    \endcode
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td> \p boost::unordered_map</td>
                    <td><tt><cds/container/striped_map/boost_unordered_map.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_map/boost_unordered_map.h>
                        #include <cds/container/refinable_hash_map.h>
                        typedef cds::container::StripedMap<
                            boost::unordered_map< Key, T, boost::hash<Key>, std::equal_to<Key> >
                        > refinable_map;
                    \endcode
                    </td>
                    <td>
                    </td>
                </tr>
            </table>


            You can use another container type as map's bucket.
            Suppose, you have a container class \p MyBestContainer and you want to integrate it with \p %StripedMap as bucket type.
            There are two possibility:
            - either your \p MyBestContainer class has native support of bucket's interface;
                in this case, you can use default <tt>striped_set::adapt</tt> metafunction;
            - or your \p MyBestContainer class does not support bucket's interface; it means you should develop a specialization
                <tt>cds::container::striped_set::adapt<MyBestContainer> </tt> metafunction providing necessary interface.

            The <tt>striped_set::adapt< Container, Options... ></tt> metafunction has two template argument:
            - \p Container is the class that should be used as the bucket, for example, <tt>std::list< std::pair< Key, T > ></tt>.
            - \p Options pack is the options from \p %StripedMap declaration. The \p adapt metafunction can use
                any option from \p Options for its internal use. For example, a \p compare option can be passed to \p adapt
                metafunction via \p Options argument of \p %StripedMap declaration.

            See \p striped_set::adapt metafunction for the description of interface that the bucket container must provide
            to be \p %StripedMap compatible.

        <b>Copy policy</b>
            There are three predefined copy policy:
            - \p cds::container::striped_set::copy_item - copy item from old bucket to new one when resizing using copy ctor. It is default policy for
                any compiler that do not support move semantics
            - \p cds::container::striped_set::move_item - move item from old bucket to new one when resizing using move semantics. It is default policy for
                any compiler that support move semantics. If compiler does not support move semantics, the move policy is the same as \p copy_item
            - \p cds::container::striped_set::swap_item - copy item from old bucket to new one when resizing using \p std::swap. Not all containers support
                this copy policy, see details in table below.

            You can define your own copy policy specifically for your case.
            Note, right copy policy can significantly improve the performance of resizing.

            <table>
                <tr>
                    <th>Container</th>
                    <th>Policies</th>
                </tr>
                <tr>
                    <td>
                        - \p std::list
                        - \p boost::list
                    </td>
                    <td>\code
                        struct copy_item {
                            void operator()(
                                std::list< std::pair<const Key, T> >& list,
                                std::list<std::pair<const Key, T> >::iterator itInsert,
                                std::list<std::pair<const Key, T> >::iterator itWhat )
                            {
                                list.insert( itInsert, *itWhat );
                            }
                        } \endcode

                        \code
                        // The type T stored in the list must be swappable
                        struct swap_item {
                            void operator()(
                                std::list< std::pair<const Key, T> >& list,
                                std::list<std::pair<const Key, T> >::iterator itInsert,
                                std::list<std::pair<const Key, T> >::iterator itWhat )
                            {
                                std::pair<Key, T> newVal( itWhat->first, T());
                                std::swap( list.insert( itInsert, newVal )->second, itWhat->second );
                            }
                        } \endcode

                        \code
                        struct move_item {
                            void operator()(
                                std::list< std::pair<const Key, T> >& list,
                                std::list<std::pair<const Key, T> >::iterator itInsert,
                                std::list<std::pair<const Key, T> >::iterator itWhat )
                            {
                                list.insert( itInsert, std::move( *itWhat ));
                            }
                        } \endcode
                    </td>
                </tr>
                <tr>
                    <td>
                        - \p std::map
                        - \p std::unordered_map
                        - \p boost::container::map
                        - \p boost::container::flat_map
                        - \p boost::unordered_map
                    </td>
                    <td>\code
                        struct copy_item {
                            void operator()( std::map< Key, T>& map, std::map<Key, T>::iterator itWhat )
                            {
                                map.insert( *itWhat );
                            }
                        } \endcode

                    \code
                        struct swap_item {
                            void operator()( std::map< Key, T>& map, std::map<Key, T>::iterator itWhat )
                            {
                                std::swap(
                                    map.insert(
                                        std::map::value_type( itWhat->first, T())).first->second
                                        , itWhat->second
                                ));
                            }
                        } \endcode
                        \p T type must be swappable.

                    \code
                        struct move_item {
                            void operator()( std::map< Key, T>& map, std::map<Key, T>::iterator itWhat )
                            {
                                map.insert( std::move( *itWhat ));
                            }
                        } \endcode
                </tr>
                <tr>
                    <td> \p boost::container::slist</td>
                    <td>\code
                        struct copy_item {
                            void operator()(
                                bc::slist< std::pair<const Key, T> >& list,
                                bc::slist<std::pair<const Key, T> >::iterator itInsert,
                                bc::slist<std::pair<const Key, T> >::iterator itWhat )
                            {
                                list.insert_after( itInsert, *itWhat );
                            }
                        } \endcode

                        \code
                        // The type T stored in the list must be swappable
                        struct swap_item {
                            void operator()(
                                bc::slist< std::pair<const Key, T> >& list,
                                bc::slist<std::pair<const Key, T> >::iterator itInsert,
                                bc::slist<std::pair<const Key, T> >::iterator itWhat )
                            {
                                std::pair<Key, T> newVal( itWhat->first, T());
                                std::swap( list.insert( itInsert, newVal )->second, itWhat->second );
                            }
                        } \endcode

                        \code
                        struct move_item {
                            void operator()(
                                bc::slist< std::pair<const Key, T> >& list,
                                bc::slist<std::pair<const Key, T> >::iterator itInsert,
                                bc::slist<std::pair<const Key, T> >::iterator itWhat )
                            {
                                list.insert_after( itInsert, std::move( *itWhat ));
                            }
                        } \endcode
                    </td>
                </tr>
            </table>

        <b>Advanced functions</b>

        The library provides some advanced functions like \p erase_with(), \p find_with(),
        that cannot be supported by all underlying containers.
        The table below shows whether underlying container supports those functions
        (the sign "+" means "container supports the function"):

        <table>
            <tr>
                <th>Container</th>
                <th>\p find_with</th>
                <th>\p erse_with</th>
            </tr>
            <tr>
                <td> \p std::list</td>
                <td>+</td>
                <td>+</td>
            </tr>
            <tr>
                <td> \p std::map</td>
                <td>-</td>
                <td>-</td>
            </tr>
            <tr>
                <td> \p std::unordered_map</td>
                <td>-</td>
                <td>-</td>
            </tr>
            <tr>
                <td> \p boost::container::slist</td>
                <td>+</td>
                <td>+</td>
            </tr>
            <tr>
                <td> \p boost::container::list</td>
                <td>+</td>
                <td>+</td>
            </tr>
            <tr>
                <td> \p boost::container::map</td>
                <td>-</td>
                <td>-</td>
            </tr>
            <tr>
                <td> \p boost::container::flat_map</td>
                <td>-</td>
                <td>-</td>
            </tr>
            <tr>
                <td> \p boost::unordered_map</td>
                <td>-</td>
                <td>-</td>
            </tr>
        </table>

    **/
template <class Container, typename... Options>
    class StripedMap
#ifdef CDS_DOXYGEN_INVOKED
        : protected StripedSet<Container, Options...>
#else
        : protected details::make_striped_map< Container, Options...>::type
#endif
    {
        //@cond
        typedef typename details::make_striped_map< Container, Options...>::type base_class;
        //@endcond

    public:
        //@cond
        typedef typename base_class::default_options    default_options;
        typedef typename base_class::options            options;
        //@endcond

        typedef Container                           underlying_container_type   ;   ///< original intrusive container type for the bucket
        typedef typename base_class::bucket_type    bucket_type ;   ///< container type adapted for hash set
        typedef typename bucket_type::value_type    value_type  ;   ///< pair type (<tt> std::pair<key_type const, mapped_type> </tt>)
        typedef typename value_type::first_type     key_type    ;   ///< key type
        typedef typename value_type::second_type    mapped_type ;   ///< mapped type

        typedef typename base_class::hash               hash            ; ///< Hash functor
        typedef typename base_class::item_counter       item_counter    ; ///< Item counter
        typedef typename base_class::resizing_policy    resizing_policy ; ///< Resizing policy
        typedef typename base_class::allocator_type     allocator_type  ; ///< allocator type specified in options.
        typedef typename base_class::mutex_policy       mutex_policy    ; ///< Mutex policy

    protected:
        //@cond
        typedef typename base_class::scoped_cell_lock   scoped_cell_lock;
        typedef typename base_class::scoped_full_lock   scoped_full_lock;
        typedef typename base_class::scoped_resize_lock scoped_resize_lock;
        //@endcond

    private:
        //@cond
        struct key_accessor {
            key_type const& operator()( value_type const& p ) const
            {
                return p.first;
            }
        };
        //@endcond

    public:
        /// Default ctor. The initial capacity is 16.
        StripedMap()
        : base_class()
        {}

        /// Ctor with initial capacity specified
        StripedMap(
            size_t nCapacity    ///< Initial size of bucket table and lock array. Must be power of two, the minimum is 16.
        ) : base_class( nCapacity )
        {}

        /// Ctor with resizing policy (copy semantics)
        /**
            This constructor initializes m_ResizingPolicy member with copy of \p resizingPolicy parameter
        */
        StripedMap(
            size_t nCapacity    ///< Initial size of bucket table and lock array. Must be power of two, the minimum is 16.
            ,resizing_policy const& resizingPolicy  ///< Resizing policy
        ) : base_class( nCapacity, resizingPolicy )
        {}

        /// Ctor with resizing policy (move semantics)
        /**
            This constructor initializes m_ResizingPolicy member moving \p resizingPolicy parameter
            Move semantics is used. Available only for the compilers that supports C++11 rvalue reference.
        */
        StripedMap(
            size_t nCapacity    ///< Initial size of bucket table and lock array. Must be power of two, the minimum is 16.
            ,resizing_policy&& resizingPolicy  ///< Resizing policy
            ) : base_class( nCapacity, std::forward<resizing_policy>(resizingPolicy))
        {}

        /// Destructor destroys internal data
        ~StripedMap()
        {}

    public:
        /// Inserts new node with key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from a value of type \p K.
                In trivial case, \p K is equal to \p key_type.
            - The \p mapped_type should be default-constructible.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( K const& key )
        {
            return insert_with( key, [](value_type&){} );
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from \p key of type \p K.
            - The \p mapped_type should be constructible from \p val of type \p V.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( K const& key, V const& val )
        {
            return insert_with( key, [&val](value_type& item) { item.second = val ; } );
        }

        /// Inserts new node and initialize it by a functor
        /**
            This function inserts new node with key \p key and if inserting is successful then it calls
            \p func functor with signature
            \code
                struct functor {
                    void operator()( value_type& item );
                };
            \endcode

            The argument \p item of user-defined functor \p func is the reference
            to the map's item inserted:
                - <tt>item.first</tt> is a const reference to item's key that cannot be changed.
                - <tt>item.second</tt> is a reference to item's value that may be changed.

            The key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.
        */
        template <typename K, typename Func>
        bool insert_with( const K& key, Func func )
        {
            return base_class::insert( key, func );
        }

        /// For key \p key inserts data of type \p mapped_type created in-place from \p args
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            bool bOk;
            bool bResize;
            size_t nHash = base_class::hashing( std::forward<K>(key));
            bucket_type * pBucket;
            {
                scoped_cell_lock sl( base_class::m_MutexPolicy, nHash );
                pBucket = base_class::bucket( nHash );

                bOk = pBucket->emplace( std::forward<K>(key), std::forward<Args>(args)...);
                bResize = bOk && base_class::m_ResizingPolicy( ++base_class::m_ItemCounter, *this, *pBucket );
            }

            if ( bResize )
                base_class::resize();

            return bOk;
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If \p key is not found in the map, then \p key is inserted iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.

            The functor signature is:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item );
                };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the map

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the map.
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K const& key, Func func, bool bAllowInsert = true )
        {
            std::pair<bool, bool> result;
            bool bResize;
            size_t nHash = base_class::hashing( key );
            bucket_type * pBucket;
            {
                scoped_cell_lock sl( base_class::m_MutexPolicy, nHash );
                pBucket = base_class::bucket( nHash );

                result = pBucket->update( key, func, bAllowInsert );
                bResize = result.first && result.second && base_class::m_ResizingPolicy( ++base_class::m_ItemCounter, *this, *pBucket );
            }

            if ( bResize )
                base_class::resize();
            return result;
        }
        //@cond
        template <typename K, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update() instead")
        std::pair<bool, bool> ensure( K const& key, Func func )
        {
            return update( key, func, true );
        }
        //@endcond

        /// Delete \p key from the map
        /** \anchor cds_nonintrusive_StripedMap_erase

            Return \p true if \p key is found and deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            return base_class::erase( key );
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_StripedMap_erase "erase(K const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the map.

            @note This function is enabled if the compiler supports C++11
            default template arguments for function template <b>and</b> the underlying container
            supports \p %erase_with feature.
        */
        template < typename K, typename Less
            ,typename Bucket = bucket_type, typename = typename std::enable_if< Bucket::has_erase_with >::type >
        bool erase_with( K const& key, Less pred )
        {
            return erase_with( key, pred, [](value_type const&) {} );
        }

        /// Delete \p key from the map
        /** \anchor cds_nonintrusive_StripedMap_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& item) { ... }
            };
            \endcode

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            return base_class::erase( key, f );
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_StripedMap_erase_func "erase(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the map.

            @note This function is enabled if the compiler supports C++11
            default template arguments for function template <b>and</b> the underlying container
            supports \p %erase_with feature.
        */
        template <typename K, typename Less, typename Func
            ,typename Bucket = bucket_type, typename = typename std::enable_if< Bucket::has_erase_with >::type >
        bool erase_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper< value_type, Less, key_accessor >(), f );
        }

        /// Find the key \p key
        /** \anchor cds_nonintrusive_StripedMap_find_func

            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            The functor may change \p item.second.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename K, typename Func>
        bool find( K const& key, Func f )
        {
            return base_class::find( key, [&f]( value_type& pair, K const& ) mutable { f(pair); } );
        }

        /// Find the key \p val using \p pred predicate
        /**
            The function is an analog of \ref cds_nonintrusive_StripedMap_find_func "find(K const&, Func)"
            but \p pred is used for key comparing
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.

            @note This function is enabled if the compiler supports C++11
            default template arguments for function template <b>and</b> the underlying container
            supports \p %find_with feature.
        */
        template <typename K, typename Less, typename Func
            ,typename Bucket = bucket_type, typename = typename std::enable_if< Bucket::has_find_with >::type >
        bool find_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper< value_type, Less, key_accessor >(),
                [&f]( value_type& pair, K const& ) mutable { f(pair); } );
        }

        /// Checks whether the map contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename K>
        bool contains( K const& key )
        {
            return base_class::contains( key );
        }
        //@cond
        template <typename K>
        CDS_DEPRECATED("use contains()")
        bool find( K const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the set contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.

            @note This function is enabled if the compiler supports C++11
            default template arguments for function template <b>and</b> the underlying container
            supports \p %contains() feature.
        */
        template <typename K, typename Less
            ,typename Bucket = bucket_type, typename = typename std::enable_if< Bucket::has_find_with >::type >
        bool contains( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::contains( key, cds::details::predicate_wrapper< value_type, Less, key_accessor >());
        }
        //@cond
        template <typename K, typename Less
            ,typename Bucket = bucket_type, typename = typename std::enable_if< Bucket::has_find_with >::type >
        CDS_DEPRECATED("use contains()")
        bool find_with( K const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Clears the map
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the map is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the map is empty.
        */
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns item count in the map
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns the size of hash table
        /**
            The hash table size is non-constant and can be increased via resizing.
        */
        size_t bucket_count() const
        {
            return base_class::bucket_count();
        }

        /// Returns lock array size
        /**
            The lock array size is constant.
        */
        size_t lock_count() const
        {
            return base_class::lock_count();
        }

        /// Returns resizing policy object
        resizing_policy& get_resizing_policy()
        {
            return base_class::get_resizing_policy();
        }

        /// Returns resizing policy (const version)
        resizing_policy const& get_resizing_policy() const
        {
            return base_class::get_resizing_policy();
        }
    };

}}  // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_STRIPED_MAP_H
