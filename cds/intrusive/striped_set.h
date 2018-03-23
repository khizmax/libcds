// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_STRIPED_SET_H
#define CDSLIB_INTRUSIVE_STRIPED_SET_H

#include <cds/intrusive/details/base.h>
#include <cds/intrusive/striped_set/adapter.h>
#include <cds/intrusive/striped_set/striping_policy.h>

namespace cds { namespace intrusive {
    /// StripedSet related definitions
    namespace striped_set {

        /** @defgroup cds_striped_resizing_policy Resizing policy for striped/refinable set/map

            Resizing policy for \p intrusive::StripedSet, \p container::StripedSet and \p container::StripedMap.
        */

    }   // namespace striped_set

    /// Striped hash set
    /** @ingroup cds_intrusive_map

        Source
            - [2008] Maurice Herlihy, Nir Shavit "The Art of Multiprocessor Programming"

        Lock striping is very simple technique.
        The set consists of the bucket table and the array of locks.
        Initially, the capacity of lock array and bucket table is the same.
        When set is resized, bucket table capacity will be doubled but lock array will not.
        The lock \p i protects each bucket \p j, where <tt> j = i mod L </tt>,
        where \p L - the size of lock array.

        Template arguments:
            - \p Container - the container class that is used as bucket table entry. The \p Container class should support
                an uniform interface described below.
            - \p Options - options

        The \p %StripedSet class does not exactly dictate the type of container that should be used as a \p Container bucket.
        Instead, the class supports different intrusive container type for the bucket, for exampe,
        \p boost::intrusive::list, \p boost::intrusive::set and others.

        Remember that \p %StripedSet class algorithm ensures sequential blocking access to its bucket through the mutex type you specify
        among \p Options template arguments.

        The \p Options are:
        - \p opt::mutex_policy - concurrent access policy.
            Available policies: \p striped_set::striping, \p striped_set::refinable.
            Default is \p %striped_set::striping.
        - \p cds::opt::hash - hash functor. Default option value see <tt>opt::v::hash_selector <opt::none></tt>
            which selects default hash functor for your compiler.
        - \p cds::opt::compare - key comparison functor. No default functor is provided.
            If the option is not specified, the \p opt::less is used.
        - \p cds::opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
        - \p cds::opt::item_counter - item counter type. Default is \p atomicity::item_counter since some operation on the counter is performed
            without locks. Note that item counting is an essential part of the set algorithm, so dummy counter like \p atomicity::empty_item_counter
            is not suitable.
        - \p cds::opt::allocator - the allocator type using for memory allocation of bucket table and lock array. Default is \ref CDS_DEFAULT_ALLOCATOR.
        - \p cds::opt::resizing_policy - the resizing policy - a functor that decides when to resize the hash set.
            Default option value depends on bucket container type:
                for sequential containers like \p boost::intrusive::list the resizing policy is <tt>cds::container::striped_set::load_factor_resizing<4> </tt>;
                for other type of containers like \p boost::intrusive::set the resizing policy is cds::container::striped_set::no_resizing.
            See \ref cds_striped_resizing_policy "available resizing policy".
            Note that the choose of resizing policy depends of \p Container type:
            for sequential containers like \p boost::intrusive::list the right policy can significantly improve performance.
            For other, non-sequential types of \p Container (like a \p boost::intrusive::set) the resizing policy is not so important.
        - \p cds::opt::buffer - an initialized buffer type used only for \p boost::intrusive::unordered_set.
            Default is <tt>cds::opt::v::initialized_static_buffer< cds::any_type, 256 > </tt>.

            \p opt::compare or \p opt::less options are used in some \p Container class for ordering.
            \p %opt::compare option has the highest priority: if \p %opt::compare is specified, \p %opt::less is not used.

            You can pass other option that would be passed to \p adapt metafunction, see below.

        <b>Internal details</b>

            The \p %StripedSet class cannot utilize the \p Container specified directly, but only its adapted variant which
            supports an unified interface. Internally, the adaptation is made via \p intrusive::striped_set::adapt metafunction that wraps bucket container
            and provides the unified bucket interface suitable for \p %StripedSet. Such adaptation is completely transparent for you -
            you don't need to call \p adapt metafunction directly, \p %StripedSet class's internal machinery itself invokes appropriate
            \p adapt metafunction specialization to adjust your \p Container container class to \p %StripedSet bucket's internal interface.
            All you need is to include a right header before <tt>striped_set.h</tt>.

            By default, <tt>intrusive::striped_set::adapt<AnyContainer, OptionPack> </tt> metafunction does not make any wrapping to \p AnyContainer,
            so, the result <tt>intrusive::striped_set::adapt<AnyContainer, OptionPack>::type </tt> is the same as \p AnyContainer.
            However, there are a lot of specializations of \p %intrusive::striped_set::adapt for \p boost::intrusive containers, see table below.
            Any of this specialization wraps corresponding container making it suitable for the set's bucket.
            Remember, you should include the proper header file for \p adapt <b>before</b> including <tt>striped_set.h</tt>.

            \note It is important to specify <tt>boost::intrusive::constant_time_size<true></tt> option
            for all \p boost::intrusive container that supports this option. Fast item counting feature is essential part of
            \p %StripedSet resizing algorithm.

            <table>
                <tr>
                    <th>Container</th>
                    <th>.h-file for \p adapt</th>
                    <th>Example</th>
                    <th>Notes</th>
                </tr>
                <tr>
                    <td> \p boost::intrusive::list</td>
                    <td><tt><cds/intrusive/striped_set/boost_list.h></tt></td>
                    <td>\code
                        #include <cds/intrusive/striped_set/boost_list.h>
                        #include <cds/intrusive/striped_set.h>
                        typedef cds::intrusive::StripedSet<
                        boost::intrusive::list<T, boost::intrusive::constant_time_size<true> >,
                            cds::opt::less< std::less<T> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        The list is ordered.
                        Template argument pack \p Options <b>must</b> contain cds::opt::less or cds::opt::compare for type \p T stored in the list
                    </td>
                </tr>
                <tr>
                    <td> \p boost::intrusive::slist</td>
                    <td><tt><cds/intrusive/striped_set/boost_slist.h></tt></td>
                    <td>\code
                        #include <cds/intrusive/striped_set/boost_slist.h>
                        #include <cds/intrusive/striped_set.h>
                        typedef cds::intrusive::StripedSet<
                            boost::intrusive::slist<T, boost::intrusive::constant_time_size<true> >,
                            cds::opt::less< std::less<T> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        The list is ordered.
                        Template argument pack \p Options <b>must</b> contain \p cds::opt::less or \p cds::opt::compare for type \p T stored in the list
                    </td>
                </tr>
                <tr>
                    <td> \p boost::intrusive::set</td>
                    <td><tt><cds/intrusive/striped_set/boost_set.h></tt></td>
                    <td>\code
                        #include <cds/intrusive/striped_set/boost_set.h>
                        #include <cds/intrusive/striped_set.h>
                        typedef cds::intrusive::StripedSet<
                            boost::intrusive::set<T, boost::intrusive::constant_time_size<true> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        Note that \p boost::intrusive::compare option using in \p boost::intrusive::set
                        should support \p T type stored in the set and any type \p Q that you can use
                        in \p erase() and \p find() member functions.
                    </td>
                </tr>
                <tr>
                    <td> \p boost::intrusive::unordered_set</td>
                    <td><tt><cds/intrusive/striped_set/boost_unordered_set.h></tt></td>
                    <td>\code
                        #include <cds/intrusive/striped_set/boost_unordered_set.h>
                        #include <cds/intrusive/striped_set.h>
                        typedef cds::intrusive::StripedSet<
                            boost::intrusive::unordered_set<T
                                ,boost::intrusive::constant_time_size<true>
                                ,boost::intrusive::hash< user_provided_hash_functor >
                            >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        You should provide two different hash function \p h1 and \p h2 - one for \p boost::intrusive::unordered_set
                        and other for \p %StripedSet. For the best result, \p h1 and \p h2 must be orthogonal i.e. <tt>h1(X) != h2(X)</tt> for any value \p X

                        The option \p opt::buffer is used for \p boost::intrusive::bucket_traits.
                        Default is <tt> cds::opt::v::initialized_static_buffer< cds::any_type, 256 > </tt>.
                        The resizing policy should correlate with the buffer capacity.
                        The default resizing policy is <tt>cds::container::striped_set::load_factor_resizing<256> </tt> what gives load factor 1 for
                        default bucket buffer that is the best for \p boost::intrusive::unordered_set.
                    </td>
                </tr>
                <tr>
                    <td> \p boost::intrusive::avl_set</td>
                    <td><tt><cds/intrusive/striped_set/boost_avl_set.h></tt></td>
                    <td>\code
                        #include <cds/intrusive/striped_set/boost_avl_set.h>
                        #include <cds/intrusive/striped_set.h>
                        typedef cds::intrusive::StripedSet<
                            boost::intrusive::avl_set<T, boost::intrusive::constant_time_size<true> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        Note that \p boost::intrusive::compare option using in \p boost::intrusive::avl_set
                        should support \p T type stored in the set and any type \p Q that you can use
                        in \p erase() and \p find() member functions.
                    </td>
                </tr>
                <tr>
                    <td> \p boost::intrusive::sg_set</td>
                    <td><tt><cds/intrusive/striped_set/boost_sg_set.h></tt></td>
                    <td>\code
                        #include <cds/intrusive/striped_set/boost_sg_set.h>
                        #include <cds/intrusive/striped_set.h>
                        typedef cds::intrusive::StripedSet<
                            boost::intrusive::sg_set<T, boost::intrusive::constant_time_size<true> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        Note that \p boost::intrusive::compare option using in \p boost::intrusive::sg_set
                        should support \p T type stored in the set and any type \p Q that you can use
                        in \p erase() and \p find() member functions.
                    </td>
                </tr>
                <tr>
                    <td> \p boost::intrusive::splay_set</td>
                    <td><tt><cds/intrusive/striped_set/boost_splay_set.h></tt></td>
                    <td>\code
                        #include <cds/intrusive/striped_set/boost_splay_set.h>
                        #include <cds/intrusive/striped_set.h>
                        typedef cds::intrusive::StripedSet<
                            boost::intrusive::splay_set<T, boost::intrusive::constant_time_size<true> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        Note that \p boost::intrusive::compare option using in \p boost::intrusive::splay_set
                        should support \p T type stored in the set and any type \p Q that you can use
                        in \p erase() and \p find() member functions.
                    </td>
                </tr>
                <tr>
                    <td> \p boost::intrusive::treap_set</td>
                    <td><tt><cds/intrusive/striped_set/boost_treap_set.h></tt></td>
                    <td>\code
                        #include <cds/intrusive/striped_set/boost_treap_set.h>
                        #include <cds/intrusive/striped_set.h>
                        typedef cds::intrusive::StripedSet<
                            boost::intrusive::treap_set<T, boost::intrusive::constant_time_size<true> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        Note that \p boost::intrusive::compare option using in \p boost::intrusive::treap_set
                        should support \p T type stored in the set and any type \p Q that you can use
                        in \p erase() and \p find() member functions.
                    </td>
                </tr>
            </table>

            You can use another intrusive container type as striped set's bucket.
            Suppose, you have a container class \p MyBestContainer and you want to integrate it with \p StripedSet as bucket type.
            There are two possibility:
            - either your \p MyBestContainer class has native support of bucket's interface;
                in this case, you can use default \p intrusive::striped_set::adapt metafunction;
            - or your \p MyBestContainer class does not support bucket's interface, which means, that you should create a specialization of
                <tt>cds::intrusive::striped_set::adapt<MyBestContainer> </tt> metafunction providing necessary interface.

            The <tt>intrusive::striped_set::adapt< Container, OptionPack ></tt> metafunction has two template argument:
            - \p Container is the class that should be used as the bucket, for example, <tt>boost::intrusive::list< T ></tt>.
            - \p OptionPack is the packed options from \p %StripedSet declaration. The \p adapt metafunction can use
                any option from \p OptionPack for its internal use. For example, a \p compare option can be passed to \p adapt
                metafunction via \p OptionPack argument of \p %StripedSet declaration.

            See \p intrusive::striped_set::adapt metafunction for the description of interface that the bucket container must provide
            to be \p %StripedSet compatible.
    */
    template <class Container, typename... Options>
    class StripedSet
    {
    public:
        //@cond
        struct default_options {
            typedef striped_set::striping<>         mutex_policy;
            typedef typename cds::opt::v::hash_selector< cds::opt::none >::type   hash;
            typedef cds::atomicity::item_counter    item_counter;
            typedef CDS_DEFAULT_ALLOCATOR           allocator;
            typedef cds::opt::none                  resizing_policy;
            typedef cds::opt::none                  compare;
            typedef cds::opt::none                  less;
        };

        typedef typename cds::opt::make_options<
            typename cds::opt::find_type_traits< default_options, Options... >::type
            ,Options...
        >::type   options;
        //@endcond

        typedef Container                           underlying_container_type   ;   ///< original intrusive container type for the bucket
        typedef typename cds::intrusive::striped_set::adapt< underlying_container_type, Options... >::type   bucket_type ;   ///< container type adapted for hash set
        typedef typename bucket_type::value_type    value_type  ;   ///< value type stored in the set

        typedef typename options::hash              hash            ; ///< Hash functor
        typedef typename options::item_counter      item_counter    ; ///< Item counter
        typedef typename cds::opt::select_default<
            typename options::resizing_policy,
            typename bucket_type::default_resizing_policy
        >::type                                     resizing_policy ; ///< Resizing policy
        typedef typename options::allocator         allocator_type  ; ///< allocator type specified in options.
        typedef typename options::mutex_policy      mutex_policy    ; ///< Mutex policy

        typedef cds::details::Allocator< bucket_type, allocator_type > bucket_allocator;  ///< bucket allocator type based on allocator_type

    protected:
        bucket_type *           m_Buckets;      ///< Bucket table
        atomics::atomic<size_t> m_nBucketMask;  ///< Bucket table size - 1. m_nBucketMask + 1 should be power of two.
        item_counter            m_ItemCounter;  ///< Item counter
        hash                    m_Hash;         ///< Hash functor

        mutex_policy    m_MutexPolicy   ;   ///< Mutex policy
        resizing_policy m_ResizingPolicy;   ///< Resizing policy

        static const size_t c_nMinimalCapacity = 16 ;   ///< Minimal capacity

    protected:
        //@cond
        typedef typename mutex_policy::scoped_cell_lock     scoped_cell_lock;
        typedef typename mutex_policy::scoped_full_lock     scoped_full_lock;
        typedef typename mutex_policy::scoped_resize_lock   scoped_resize_lock;
        //@endcond

    protected:
        //@cond
        static size_t calc_init_capacity( size_t nCapacity )
        {
            nCapacity = cds::beans::ceil2( nCapacity );
            return nCapacity < c_nMinimalCapacity ? c_nMinimalCapacity : nCapacity;
        }

        void alloc_bucket_table( size_t nSize )
        {
            assert( cds::beans::is_power2( nSize ));
            m_nBucketMask.store( nSize - 1, atomics::memory_order_release );
            m_Buckets = bucket_allocator().NewArray( nSize );
        }

        static void free_bucket_table( bucket_type * pBuckets, size_t nSize )
        {
            bucket_allocator().Delete( pBuckets, nSize );
        }

        template <typename Q>
        size_t hashing( Q const& v ) const
        {
            return m_Hash( v );
        }

        bucket_type * bucket( size_t nHash ) const noexcept
        {
            return m_Buckets + (nHash & m_nBucketMask.load( atomics::memory_order_relaxed ));
        }

        template <typename Q, typename Func>
        bool find_( Q& val, Func f )
        {
            size_t nHash = hashing( val );

            scoped_cell_lock sl( m_MutexPolicy, nHash );
            return bucket( nHash )->find( val, f );
        }

        template <typename Q, typename Less, typename Func>
        bool find_with_( Q& val, Less pred, Func f )
        {
            size_t nHash = hashing( val );
            scoped_cell_lock sl( m_MutexPolicy, nHash );
            return bucket( nHash )->find( val, pred, f );
        }

        void internal_resize( size_t nNewCapacity )
        {
            // All locks are already locked!
            m_MutexPolicy.resize( nNewCapacity );

            size_t nOldCapacity = bucket_count();
            bucket_type * pOldBuckets = m_Buckets;

            alloc_bucket_table( nNewCapacity );

            typedef typename bucket_type::iterator bucket_iterator;
            bucket_type * pEnd = pOldBuckets + nOldCapacity;
            for ( bucket_type * pCur = pOldBuckets; pCur != pEnd; ++pCur ) {
                bucket_iterator itEnd = pCur->end();
                bucket_iterator itNext;
                for ( bucket_iterator it = pCur->begin(); it != itEnd; it = itNext ) {
                    itNext = it;
                    ++itNext;
                    bucket( m_Hash( *it ))->move_item( *pCur, it );
                }
                pCur->clear();
            }

            free_bucket_table( pOldBuckets, nOldCapacity );

            m_ResizingPolicy.reset();
        }

        void resize()
        {
            size_t nOldCapacity = bucket_count( atomics::memory_order_acquire );

            scoped_resize_lock al( m_MutexPolicy );
            if ( al.success()) {
                if ( nOldCapacity != bucket_count( atomics::memory_order_acquire )) {
                    // someone resized already
                    return;
                }

                internal_resize( nOldCapacity * 2 );
            }
        }

        //@endcond

    public:
        /// Default ctor. The initial capacity is 16.
        StripedSet()
            : m_Buckets( nullptr )
            , m_nBucketMask( c_nMinimalCapacity - 1 )
            , m_MutexPolicy( c_nMinimalCapacity )
        {
            alloc_bucket_table( bucket_count());
        }

        /// Ctor with initial capacity specified
        StripedSet(
            size_t nCapacity    ///< Initial size of bucket table and lock array. Must be power of two, the minimum is 16.
        )
            : m_Buckets( nullptr )
            , m_nBucketMask( calc_init_capacity(nCapacity) - 1 )
            , m_MutexPolicy( bucket_count())
        {
            alloc_bucket_table( bucket_count());
        }

        /// Ctor with resizing policy (copy semantics)
        /**
            This constructor initializes m_ResizingPolicy member with copy of \p resizingPolicy parameter
        */
        StripedSet(
            size_t nCapacity    ///< Initial size of bucket table and lock array. Must be power of two, the minimum is 16.
            ,resizing_policy const& resizingPolicy  ///< Resizing policy
        )
        : m_Buckets( nullptr )
        , m_nBucketMask( ( nCapacity ? calc_init_capacity(nCapacity) : c_nMinimalCapacity ) - 1 )
        , m_MutexPolicy( bucket_count())
        , m_ResizingPolicy( resizingPolicy )
        {
            alloc_bucket_table( bucket_count());
        }

        /// Ctor with resizing policy (move semantics)
        /**
            This constructor initializes m_ResizingPolicy member moving \p resizingPolicy parameter
            Move semantics is used.
        */
        StripedSet(
            size_t nCapacity    ///< Initial size of bucket table and lock array. Must be power of two, the minimum is 16.
            ,resizing_policy&& resizingPolicy  ///< Resizing policy
        )
        : m_Buckets( nullptr )
        , m_nBucketMask( ( nCapacity ? calc_init_capacity(nCapacity) : c_nMinimalCapacity ) - 1 )
        , m_MutexPolicy( bucket_count())
        , m_ResizingPolicy( std::forward<resizing_policy>( resizingPolicy ))
        {
            alloc_bucket_table( bucket_count());
        }

        /// Destructor destroys internal data
        ~StripedSet()
        {
            free_bucket_table( m_Buckets, bucket_count());
        }

    public:
        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with key equal to \p val.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert( val, []( value_type& ) {} );
        }

        /// Inserts new node
        /**
            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls  \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted.
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            bool bOk;
            bool bResize;
            size_t nHash = hashing( val );
            bucket_type * pBucket;
            {
                scoped_cell_lock sl( m_MutexPolicy, nHash );
                pBucket = bucket( nHash );
                bOk = pBucket->insert( val, f );
                bResize = bOk && m_ResizingPolicy( ++m_ItemCounter, *this, *pBucket );
            }

            if ( bResize )
                resize();
            return bOk;
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the set, then \p val is inserted
            iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor signature is:
            \code
                void func( bool bNew, value_type& item, value_type& val );
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p update() function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refers to the same thing.

            The functor may change non-key fields of the \p item.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p val
            already is in the set.
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
        {
            std::pair<bool, bool> result;
            bool bResize;
            size_t nHash = hashing( val );
            bucket_type * pBucket;
            {
                scoped_cell_lock sl( m_MutexPolicy, nHash );
                pBucket = bucket( nHash );

                result = pBucket->update( val, func, bAllowInsert );
                bResize = result.first && result.second && m_ResizingPolicy( ++m_ItemCounter, *this, *pBucket );
            }

            if ( bResize )
                resize();
            return result;
        }
        //@cond
        template <typename Func>
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Unlink the item \p val from the set
        /**
            The function searches the item \p val in the set and unlink it
            if it is found and is equal to \p val (here, the equality means that
            \p val belongs to the set: if \p item is an item found then
            unlink is successful iif <tt>&val == &item</tt>)

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            bool bOk;
            size_t nHash = hashing( val );
            {
                scoped_cell_lock sl( m_MutexPolicy, nHash );
                bOk = bucket( nHash )->unlink( val );
            }

            if ( bOk )
                --m_ItemCounter;
            return bOk;
        }

        /// Deletes the item from the set
        /** \anchor cds_intrusive_StripedSet_erase
            The function searches an item with key equal to \p val in the set,
            unlinks it from the set, and returns a pointer to unlinked item.

            If the item with key equal to \p val is not found the function return \p nullptr.

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        value_type * erase( Q const& val )
        {
            return erase( val, [](value_type const&) {} );
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_StripedSet_erase "erase(Q const&)"
            but \p pred is used for key comparing
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        value_type * erase_with( Q const& val, Less pred )
        {
            return erase_with( val, pred, [](value_type const&) {} );
        }

        /// Deletes the item from the set
        /** \anchor cds_intrusive_StripedSet_erase_func

            The function searches an item with key equal to \p val in the set,
            call \p f functor with item found, unlinks it from the set, and returns a pointer to unlinked item.

            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode

            If the item with key equal to \p val is not found the function return \p false.

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q, typename Func>
        value_type * erase( Q const& val, Func f )
        {
            size_t nHash = hashing( val );
            value_type * pVal;
            {
                scoped_cell_lock sl( m_MutexPolicy, nHash );
                pVal = bucket( nHash )->erase( val, f );
            }

            if ( pVal )
                --m_ItemCounter;
            return pVal;
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_StripedSet_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        value_type * erase_with( Q const& val, Less pred, Func f )
        {
            size_t nHash = hashing( val );
            value_type * pVal;
            {
                scoped_cell_lock sl( m_MutexPolicy, nHash );
                pVal = bucket( nHash )->erase( val, pred, f );
            }

            if ( pVal )
                --m_ItemCounter;
            return pVal;
        }

        /// Find the key \p val
        /** \anchor cds_intrusive_StripedSet_find_func
            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            The functor may change non-key fields of \p item.

            The \p val argument is non-const since it can be used as \p f functor destination i.e., the functor
            may modify both arguments.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& val, Func f )
        {
            return find_( val, f );
        }

        /// Find the key \p val using \p pred predicate
        /**
            The function is an analog of \ref cds_intrusive_StripedSet_find_func "find(Q&, Func)"
            but \p pred is used for key comparing
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& val, Less pred, Func f )
        {
            return find_with_( val, pred, f );
        }

        /// Find the key \p val
        /** \anchor cds_intrusive_StripedSet_find_cfunc
            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q const& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            The functor may change non-key fields of \p item.

            The \p val argument is non-const since it can be used as \p f functor destination i.e., the functor
            may modify both arguments.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q const& val, Func f )
        {
            return find_( val, f );
        }

        /// Find the key \p val using \p pred predicate
        /**
            The function is an analog of \ref cds_intrusive_StripedSet_find_cfunc "find(Q const&, Func)"
            but \p pred is used for key comparing
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& val, Less pred, Func f )
        {
            return find_with_( val, pred, f );
        }

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
            Otherwise, you may use \p contains( Q const&, Less pred ) functions with explicit predicate for key comparing.
        */
        template <typename Q>
        bool contains( Q const& key )
        {
            return find( key, [](value_type&, Q const& ) {} );
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("use contains()")
        bool find( Q const& val )
        {
            return contains( val );
        }
        //@endcond

        /// Checks whether the set contains \p key using \p pred predicate for searching
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            return find_with( key, pred, [](value_type& , Q const& ) {} );
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("use contains()")
        bool find_with( Q const& val, Less pred )
        {
            return contains( val, pred );
        }
        //@endcond

        /// Clears the set
        /**
            The function unlinks all items from the set.
        */
        void clear()
        {
            // locks entire array
            scoped_full_lock sl( m_MutexPolicy );

            size_t nBucketCount = bucket_count();
            bucket_type * pBucket = m_Buckets;
            for ( size_t i = 0; i < nBucketCount; ++i, ++pBucket )
                pBucket->clear();
            m_ItemCounter.reset();
        }

        /// Clears the set and calls \p disposer for each item
        /**
            The function unlinks all items from the set calling \p disposer for each item.
            \p Disposer functor interface is:
            \code
            struct Disposer{
                void operator()( value_type * p );
            };
            \endcode
        */
        template <typename Disposer>
        void clear_and_dispose( Disposer disposer )
        {
            // locks entire array
            scoped_full_lock sl( m_MutexPolicy );

            size_t nBucketCount = bucket_count();
            bucket_type * pBucket = m_Buckets;
            for ( size_t i = 0; i < nBucketCount; ++i, ++pBucket )
                pBucket->clear( disposer );
            m_ItemCounter.reset();
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
        */
        bool empty() const
        {
            return size() == 0;
        }

        /// Returns item count in the set
        size_t size() const
        {
            return m_ItemCounter;
        }

        /// Returns the size of hash table
        /**
            The hash table size is non-constant and can be increased via resizing.
        */
        size_t bucket_count() const
        {
            return m_nBucketMask.load( atomics::memory_order_relaxed ) + 1;
        }
        //@cond
        size_t bucket_count( atomics::memory_order load_mo ) const
        {
            return m_nBucketMask.load( load_mo ) + 1;
        }
        //@endcond

        /// Returns lock array size
        size_t lock_count() const
        {
            return m_MutexPolicy.lock_count();
        }

        /// Returns resizing policy object
        resizing_policy& get_resizing_policy()
        {
            return m_ResizingPolicy;
        }

        /// Returns resizing policy (const version)
        resizing_policy const& get_resizing_policy() const
        {
            return m_ResizingPolicy;
        }
    };
}}  // namespace cds::itrusive

#endif // #ifndef CDSLIB_INTRUSIVE_STRIPED_SET_H
