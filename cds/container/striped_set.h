// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_STRIPED_SET_H
#define CDSLIB_CONTAINER_STRIPED_SET_H

#include <cds/intrusive/striped_set.h>
#include <cds/container/striped_set/adapter.h>

namespace cds { namespace container {

    /// Striped hash set
    /** @ingroup cds_nonintrusive_set

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
        Instead, the class supports different container type for the bucket, for exampe, \p std::list, \p std::set and others.

        Remember that \p %StripedSet class algorithm ensures sequential blocking access to its bucket through the mutex type you specify
        among \p Options template arguments.

        The \p Options are:
            - \p opt::mutex_policy - concurrent access policy.
                Available policies: \p intrusive::striped_set::striping, \p intrusive::striped_set::refinable.
                Default is \p %striped_set::striping.
            - \p opt::hash - hash functor. Default option value see <tt>opt::v::hash_selector<opt::none> </tt>
                which selects default hash functor for your compiler.
            - \p opt::compare - key comparison functor. No default functor is provided.
                If the option is not specified, the \p %opt::less is used.
            - \p opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
            - \p opt::item_counter - item counter type. Default is \p atomicity::item_counter since some operation on the counter is performed
                without locks. Note that item counting is an essential part of the set algorithm, so dummy counter
                like as \p atomicity::empty_item_counter is not suitable.
            - \p opt::allocator - the allocator type using for memory allocation of bucket table and lock array. Default is \ref CDS_DEFAULT_ALLOCATOR.
            - \p opt::resizing_policy - the resizing policy that is a functor that decides when to resize the hash set.
                Default option value depends on bucket container type:
                    for sequential containers like \p std::list, \p std::vector the resizing policy is <tt>striped_set::load_factor_resizing<4> </tt>;
                    for other type of containers like \p std::set, \p std::unordered_set the resizing policy is \p striped_set::no_resizing.
                See \ref cds_striped_resizing_policy "available resizing policy".
                Note that the choose of resizing policy depends of \p Container type:
                for sequential containers like \p std::list, \p std::vector and so on, right choosing of the policy can
                significantly improve performance.
                For other, non-sequential types of \p Container (like a \p std::set) the resizing policy is not so important.
            - \p opt::copy_policy - the copy policy which is used to copy items from the old set to the new one when resizing.
                The policy can be optionally used in adapted bucket container for performance reasons of resizing.
                The detail of copy algorithm depends on type of bucket container and explains below.

            \p %opt::compare or \p %opt::less options are used in some \p Container class for searching an item.
            \p %opt::compare option has the highest priority: if \p %opt::compare is specified, \p %opt::less is not used.

        You can pass other option that would be passed to <tt>adapt</tt> metafunction, see below.

        <b>Internal details</b>

            The \p %StripedSet class cannot utilize the \p Container container specified directly, but only its adapted variant which
            supports an unified interface. Internally, the adaptation is made via striped_set::adapt metafunction that wraps bucket container
            and provides the unified bucket interface suitable for \p %StripedSet. Such adaptation is completely transparent for you -
            you don't need to call \p adapt metafunction directly, \p %StripedSet class's internal machinery itself invokes appropriate
            \p adapt metafunction to adjust your \p Container container class to \p %StripedSet bucket's internal interface.
            All you need is to include a right header before <tt>striped_hash_set.h</tt>.

            By default, <tt>striped_set::adapt<AnyContainer, Options...> </tt> metafunction does not make any wrapping to \p AnyContainer,
            so, the result <tt>striped_set::adapt<AnyContainer, Options...>::type </tt> is the same as \p AnyContainer.
            However, there are a lot of specializations of <tt>striped_set::adapt</tt> for well-known containers, see table below.
            Any of this specialization wraps corresponding container making it suitable for the set's bucket.
            Remember, you should include the proper header file for \p adapt <b>before</b> including <tt>striped_hash_set.h</tt>.
            <table>
                <tr>
                    <th>Container</th>
                    <th>.h-file for \p adapt</th>
                    <th>Example</th>
                    <th>Notes</th>
                </tr>
                <tr>
                    <td> \p std::list</td>
                    <td><tt><cds/container/striped_set/std_list.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_set/std_list.h>
                        #include <cds/container/striped_hash_set.h>
                        typedef cds::container::StripedSet<
                            std::list<T>,
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
                    <td> \p std::vector</td>
                    <td><tt><cds/container/striped_set/std_vector.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_set/std_vector.h>
                        #include <cds/container/striped_hash_set.h>
                        typedef cds::container::StripedSet<
                            std::vector<T>,
                            cds::opt::less< std::less<T> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        The vector is ordered.
                        Template argument pack \p Options <b>must</b> contain \p cds::opt::less or \p cds::opt::compare for type \p T stored in the list
                    </td>
                </tr>
                <tr>
                    <td> \p std::set</td>
                    <td><tt><cds/container/striped_set/std_set.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_set/std_set.h>
                        #include <cds/container/striped_hash_set.h>
                        typedef cds::container::StripedSet<
                            std::set< T, std::less<T> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td> \p std::unordered_set</td>
                    <td><tt><cds/container/striped_set/std_hash_set.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_set/std_hash_set.h>
                        #include <cds/container/striped_hash_set.h>
                        typedef cds::container::StripedSet<
                            std::unordered_set<
                                T,
                                hash<T>,
                                equal<T>
                            >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        You should provide two different hash function \p h1 and \p h2 - one for \p std::unordered_set and other for \p %StripedSet.
                        For the best result, \p h1 and \p h2 must be orthogonal i.e. <tt> h1(X) != h2(X) </tt> for any value \p X.
                    </td>
                </tr>
                <tr>
                    <td> \p boost::container::slist</td>
                    <td><tt><cds/container/striped_set/boost_slist.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_set/boost_slist.h>
                        #include <cds/container/striped_hash_set.h>
                        typedef cds::container::StripedSet<
                            boost::container::slist<T>
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        The list is ordered.
                        \p Options <b>must</b> contain \p cds::opt::less or \p cds::opt::compare.
                    </td>
                </tr>
                <tr>
                    <td> \p boost::container::list</td>
                    <td><tt><cds/container/striped_set/boost_list.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_set/boost_list.h>
                        #include <cds/container/striped_hash_set.h>
                        typedef cds::container::StripedSet<
                            boost::container::list<T>
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        The list is ordered.
                        \p Options <b>must</b> contain \p cds::opt::less or \p cds::opt::compare.
                    </td>
                </tr>
                <tr>
                    <td> \p boost::container::vector</td>
                    <td><tt><cds/container/striped_set/boost_vector.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_set/boost_vector.h>
                        #include <cds/container/striped_hash_set.h>
                        typedef cds::container::StripedSet<
                            boost::container::vector<T>,
                            cds::opt::less< std::less<T> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        The vector is ordered.
                        Template argument pack \p Options <b>must</b> contain \p cds::opt::less or \p cds::opt::compare for type \p T stored in the vector
                    </td>
                </tr>
                <tr>
                    <td> \p boost::container::stable_vector</td>
                    <td><tt><cds/container/striped_set/boost_stable_vector.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_set/boost_stable_vector.h>
                        #include <cds/container/striped_hash_set.h>
                        typedef cds::container::StripedSet<
                            boost::container::stable_vector<T>,
                            cds::opt::less< std::less<T> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        The vector is ordered.
                        Template argument pack \p Options <b>must</b> contain \p cds::opt::less or \p cds::opt::compare for type \p T stored in the vector
                    </td>
                </tr>
                <tr>
                    <td> \p boost::container::set</td>
                    <td><tt><cds/container/striped_set/boost_set.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_set/boost_set.h>
                        #include <cds/container/striped_hash_set.h>
                        typedef cds::container::StripedSet<
                            boost::container::set< T, std::less<T> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td> \p boost::container::flat_set</td>
                    <td><tt><cds/container/striped_set/boost_flat_set.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_set/boost_flat_set.h>
                        #include <cds/container/striped_hash_set.h>
                        typedef cds::container::StripedSet<
                            boost::container::flat_set< T, std::less<T> >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td> \p boost::unordered_set</td>
                    <td><tt><cds/container/striped_set/boost_unordered_set.h></tt></td>
                    <td>\code
                        #include <cds/container/striped_set/boost_unordered_set.h>
                        #include <cds/container/striped_hash_set.h>
                        typedef cds::container::StripedSet<
                            boost::unordered_set<
                                T,
                                hash<T>,
                                equal<T>
                            >
                        > striped_set;
                    \endcode
                    </td>
                    <td>
                        You should provide two different hash function \p h1 and \p h2 - one for \p boost::unordered_set and other for \p %StripedSet.
                        For the best result, \p h1 and \p h2 must be orthogonal i.e. <tt> h1(X) != h2(X) </tt> for any value \p X.
                    </td>
                </tr>
            </table>

            You can use another container type as set's bucket.
            Suppose, you have a container class \p MyBestContainer and you want to integrate it with \p %StripedSet as bucket type.
            There are two possibility:
            - either your \p MyBestContainer class has native support of bucket's interface;
                in this case, you can use default <tt>striped_set::adapt</tt> metafunction;
            - or your \p MyBestContainer class does not support bucket's interface, which means, that you should develop a specialization
                <tt>cds::container::striped_set::adapt<MyBestContainer> </tt> metafunction providing necessary interface.

            The <tt>striped_set::adapt< Container, Options... ></tt> metafunction has two template argument:
            - \p Container is the class that should be used as the bucket, for example, <tt>std::list< T ></tt>.
            - \p Options pack is the options from \p %StripedSet declaration. The \p adapt metafunction can use
                any option from \p Options for its internal use. For example, a \p compare option can be passed to \p adapt
                metafunction via \p Options argument of \p %StripedSet declaration.

            See striped_set::adapt metafunction for the description of interface that the bucket container must provide
            to be %StripedSet compatible.

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
                        - \p std::vector
                        - \p boost::list
                        - \p boost::vector
                        - \p boost::stable_vector
                    </td>
                    <td>\code
                        struct copy_item {
                            void operator()( std::list<T>& list, std::list<T>::iterator itInsert, std::list<T>::iterator itWhat )
                            {
                                list.insert( itInsert, *itWhat );
                            }
                        } \endcode

                        \code
                        // The type T stored in the list must be swappable
                        struct swap_item {
                            void operator()( std::list<T>& list, std::list<T>::iterator itInsert, std::list<T>::iterator itWhat )
                            {
                                std::swap( *list.insert( itInsert, T()), *itWhat );
                            }
                        } \endcode

                        \code
                        struct move_item {
                            void operator()( std::list<T>& list, std::list<T>::iterator itInsert, std::list<T>::iterator itWhat )
                            {
                                list.insert( itInsert, std::move( *itWhat ));
                            }
                        } \endcode
                    </td>
                </tr>
                <tr>
                    <td>
                        - \p std::set
                        - \p std::unordered_set
                    </td>
                    <td>\code
                        struct copy_item {
                            void operator()( std::set<T>& set, std::set<T>::iterator itWhat )
                            {
                                set.insert( *itWhat );
                            }
                        } \endcode
                    \p swap_item is not applicable (same as \p copy_item)

                    \code
                        struct move_item {
                            void operator()( std::set<T>& set, std::set<T>::iterator itWhat )
                            {
                                set.insert( std::move( *itWhat ));
                            }
                        } \endcode
                </tr>
                <tr>
                    <td>
                        - \p boost::container::slist
                    </td>
                    <td>\code
                        struct copy_item {
                            void operator()( bc::slist<T>& list, bc::slist<T>::iterator itInsert, bc::slist<T>::iterator itWhat )
                            {
                                list.insert_after( itInsert, *itWhat );
                            }
                        } \endcode

                        \code
                        // The type T stored in the list must be swappable
                        struct swap_item {
                            void operator()( bc::slist<T>& list, bc::slist<T>::iterator itInsert, bc::slist<T>::iterator itWhat )
                            {
                                std::swap( *list.insert_after( itInsert, T()), *itWhat );
                            }
                        } \endcode

                        \code
                        struct move_item {
                            void operator()( bc::slist<T>& list, bc::slist<T>::iterator itInsert, bc::slist<T>::iterator itWhat )
                            {
                                list.insert_after( itInsert, std::move( *itWhat ));
                            }
                        } \endcode
                    </td>
                </tr>
            </table>

        <b>Advanced functions</b>

            <b>libcds</b> provides some advanced functions like \p erase_with(), \p find_with(),
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
                    <td> \p std::vector</td>
                    <td>+</td>
                    <td>+</td>
                </tr>
                <tr>
                    <td> \p std::set</td>
                    <td>-</td>
                    <td>-</td>
                </tr>
                <tr>
                    <td> \p std::unordered_set</td>
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
                    <td> \p boost::container::vector</td>
                    <td>+</td>
                    <td>+</td>
                </tr>
                <tr>
                    <td> \p boost::container::stable_vector</td>
                    <td>+</td>
                    <td>+</td>
                </tr>
                <tr>
                    <td> \p boost::container::set</td>
                    <td>-</td>
                    <td>-</td>
                </tr>
                <tr>
                    <td> \p boost::container::flat_set</td>
                    <td>-</td>
                    <td>-</td>
                </tr>
                <tr>
                    <td> \p boost::unordered_set</td>
                    <td>-</td>
                    <td>-</td>
                </tr>
            </table>
    */
    template <class Container, typename... Options>
    class StripedSet: protected intrusive::StripedSet<Container, Options...>
    {
        //@cond
        typedef intrusive::StripedSet<Container, Options...>  base_class;
        //@endcond
    public:
        //@cond
        typedef typename base_class::default_options    default_options;
        typedef typename base_class::options            options;
        //@endcond

        typedef Container                           underlying_container_type   ;   ///< original intrusive container type for the bucket
        typedef typename base_class::bucket_type    bucket_type ;   ///< container type adapted for hash set
        typedef typename bucket_type::value_type    value_type  ;   ///< value type stored in the set

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

    public:
        /// Default ctor. The initial capacity is 16.
        StripedSet()
            : base_class()
        {}

        /// Ctor with initial capacity specified
        StripedSet(
            size_t nCapacity    ///< Initial size of bucket table and lock array. Must be power of two, the minimum is 16.
        )

            : base_class( nCapacity )
        {}

        /// Ctor with resizing policy (copy semantics)
        /**
            This constructor initializes m_ResizingPolicy member with copy of \p resizingPolicy parameter
        */
        StripedSet(
            size_t nCapacity    ///< Initial size of bucket table and lock array. Must be power of two, the minimum is 16.
            ,resizing_policy const& resizingPolicy  ///< Resizing policy
        )

            : base_class( nCapacity, resizingPolicy )
        {}

        /// Ctor with resizing policy (move semantics)
        /**
            This constructor initializes m_ResizingPolicy member moving \p resizingPolicy parameter
            Move semantics is used. Available only for the compilers that supports C++11 rvalue reference.
        */
        StripedSet(
            size_t nCapacity    ///< Initial size of bucket table and lock array. Must be power of two, the minimum is 16.
            ,resizing_policy&& resizingPolicy  ///< Resizing policy
        )

            : base_class( nCapacity, std::forward<resizing_policy>(resizingPolicy))
        {}

        /// Destructor destroys internal data
        ~StripedSet()
        {}

    public:
        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the set.

            The type \p Q should contain as minimum the complete key for the node.
            The object of \p value_type should be constructible from a value of type \p Q.
            In trivial case, \p Q is equal to \p value_type.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q const& val )
        {
            return insert( val, []( value_type& ) {} );
        }

        /// Inserts new node
        /**
            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls \p f functor to initialize value-field of new item .

            The functor signature is:
            \code
                void func( value_type& item );
            \endcode
            where \p item is the item inserted.

            The type \p Q can differ from \p value_type of items storing in the set.
            Therefore, the \p value_type should be constructible from type \p Q.

            The user-defined functor is called only if the inserting is success.
        */
        template <typename Q, typename Func>
        bool insert( Q const& val, Func f )
        {
            bool bOk;
            bool bResize;
            size_t nHash = base_class::hashing( val );
            bucket_type * pBucket;
            {
                scoped_cell_lock sl( base_class::m_MutexPolicy, nHash );
                pBucket = base_class::bucket( nHash );
                bOk = pBucket->insert( val, f );
                bResize = bOk && base_class::m_ResizingPolicy( ++base_class::m_ItemCounter, *this, *pBucket );
            }

            if ( bResize )
                base_class::resize();
            return bOk;
        }

        /// Inserts data of type \p %value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            bool bOk;
            bool bResize;
            value_type val( std::forward<Args>( args )... );
            size_t nHash = base_class::hashing( val );
            bucket_type * pBucket;
            {
                scoped_cell_lock sl( base_class::m_MutexPolicy, nHash );
                pBucket = base_class::bucket( nHash );

                bOk = pBucket->emplace( std::move( val ));
                bResize = bOk && base_class::m_ResizingPolicy( ++base_class::m_ItemCounter, *this, *pBucket );
            }

            if ( bResize )
                base_class::resize();
            return bOk;
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If \p key is not found in the set, then \p key is inserted iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.

            The functor signature is:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item, const Q& val );
                };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p %update() function

            The functor may change non-key fields of the \p item.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the map.
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> update( Q const& val, Func func, bool bAllowInsert = true )
        {
            std::pair<bool, bool> result;
            bool bResize = false;
            size_t nHash = base_class::hashing( val );
            bucket_type * pBucket;
            {
                scoped_cell_lock sl( base_class::m_MutexPolicy, nHash );
                pBucket = base_class::bucket( nHash );

                result = pBucket->update( val, func, bAllowInsert );
                if ( result.first && result.second )
                    bResize = base_class::m_ResizingPolicy( ++base_class::m_ItemCounter, *this, *pBucket );
            }

            if ( bResize )
                base_class::resize();
            return result;
        }
        //@cond
        template <typename Q, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( Q const& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Delete \p key from the set
        /** \anchor cds_nonintrusive_StripedSet_erase

            The set item comparator should be able to compare the type \p value_type and the type \p Q.
            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase( key, [](value_type const&) {} );
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_StripedSet_erase "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.

            @note This function is enabled if the compiler supports C++11
            default template arguments for function template <b>and</b> the underlying container
            supports \p %erase_with feature.
        */
        template < typename Q, typename Less
            ,typename Bucket = bucket_type, typename = typename std::enable_if< Bucket::has_erase_with >::type >
        bool erase_with( Q const& key, Less pred )
        {
            return erase_with( key, pred, [](value_type const&) {} );
        }

        /// Delete \p key from the set
        /** \anchor cds_nonintrusive_StripedSet_erase_func

            The function searches an item with key \p key, calls \p f functor with item found
            and deletes it. If \p key is not found, the functor is not called.

            The functor \p Func interface is:
            \code
            struct functor {
                void operator()(value_type const& val);
            };
            \endcode

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            bool bOk;
            size_t nHash = base_class::hashing( key );
            {
                scoped_cell_lock sl( base_class::m_MutexPolicy, nHash );
                bucket_type * pBucket = base_class::bucket( nHash );

                bOk = pBucket->erase( key, f );
            }

            if ( bOk )
                --base_class::m_ItemCounter;
            return bOk;
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_StripedSet_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.

            @note This function is enabled if the compiler supports C++11
            default template arguments for function template <b>and</b> the underlying container
            supports \p %erase_with feature.
        */
        template < typename Q, typename Less, typename Func
            , typename Bucket = bucket_type, typename = typename std::enable_if< Bucket::has_erase_with >::type >
        bool erase_with( Q const& key, Less pred, Func f )
        {
            bool bOk;
            size_t nHash = base_class::hashing( key );
            {
                scoped_cell_lock sl( base_class::m_MutexPolicy, nHash );
                bucket_type * pBucket = base_class::bucket( nHash );

                bOk = pBucket->erase( key, pred, f );
            }

            if ( bOk )
                --base_class::m_ItemCounter;
            return bOk;
        }

        /// Find the key \p val
        /** \anchor cds_nonintrusive_StripedSet_find_func

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

            The type \p Q can differ from \p value_type of items storing in the container.
            Therefore, the \p value_type should be comparable with type \p Q.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& val, Func f )
        {
            return base_class::find( val, f );
        }

        /// Find the key \p val using \p pred predicate
        /**
            The function is an analog of \ref cds_nonintrusive_StripedSet_find_func "find(Q&, Func)"
            but \p pred is used for key comparing
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.

            @note This function is enabled if the compiler supports C++11
            default template arguments for function template <b>and</b> the underlying container
            supports \p %find_with feature.
        */
        template <typename Q, typename Less, typename Func
            , typename Bucket = bucket_type, typename = typename std::enable_if< Bucket::has_find_with >::type >
        bool find_with( Q& val, Less pred, Func f )
        {
            return base_class::find_with( val, pred, f );
        }

        /// Find the key \p val
        /** \anchor cds_nonintrusive_StripedSet_find_cfunc

            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q const& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            The functor can change non-key fields of \p item.

            The type \p Q can differ from \p value_type of items storing in the container.
            Therefore, the \p value_type should be comparable with type \p Q.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q const& val, Func f )
        {
            return base_class::find( val, f );
        }

        /// Find the key \p val using \p pred predicate
        /**
            The function is an analog of \ref cds_nonintrusive_StripedSet_find_cfunc "find(Q const&, Func)"
            but \p pred is used for key comparing
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.

            @note This function is enabled if the compiler supports C++11
            default template arguments for function template <b>and</b> the underlying container
            supports \p %find_with feature.
        */
        template <typename Q, typename Less, typename Func
            , typename Bucket = bucket_type, typename = typename std::enable_if< Bucket::has_find_with >::type >
        bool find_with( Q const& val, Less pred, Func f )
        {
            return base_class::find_with( val, pred, f );
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
            return base_class::contains( key );
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("use contains()")
        bool find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename Q, typename Less
            , typename Bucket = bucket_type, typename = typename std::enable_if< Bucket::has_find_with >::type >
        bool contains( Q const& key, Less pred )
        {
            return base_class::contains( key, pred );
        }
        //@cond
        template <typename Q, typename Less
            , typename Bucket = bucket_type, typename = typename std::enable_if< Bucket::has_find_with >::type >
        CDS_DEPRECATED("use contains()")
        bool find_with( Q const& val, Less pred )
        {
            return contains( val, pred );
        }
        //@endcond

        /// Clears the set
        /**
            The function erases all items from the set.
        */
        void clear()
        {
            return base_class::clear();
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
        */
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns item count in the set
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

}} // namespace cds::container


#endif // #ifndef CDSLIB_CONTAINER_STRIPED_SET_H
