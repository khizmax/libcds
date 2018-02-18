// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OPT_OPTIONS_H
#define CDSLIB_OPT_OPTIONS_H

/*
    Framework to define template options

    Editions:
        2011.01.23 khizmax  Created
*/

#include <cstdlib> // rand, srand

#include <cds/details/aligned_type.h>
#include <cds/user_setup/allocator.h>
#include <cds/user_setup/cache_line.h>
#include <cds/algo/atomic.h>

namespace cds {

/// Framework to define template options
/**
    There are two kind of options:
    - \p type-option - option that determines a data type. The template argument \p Type of the option is a type.
    - \p value-option - option that determines a value. The template argument \p Value of the option is a value.
*/
namespace opt {

    /// Type indicates that an option is not specified and the default one should be used
    struct none
    {
        //@cond
        template <class Base> struct pack: public Base
        {};
        //@endcond
    };

    /// Metafunction for selecting default option value
    /**
        Template parameters:
        - \p Option - option value
        - \p Default - default option value
        - \p Value - option value if \p Option is not opt::none

        If \p Option is opt::none, the metafunction result is \p Default, otherwise
        the result is \p Value.

        Examples:
        \code
        // default_spin is cds::sync::spin
        typedef typename cds::opt::select_default< cds::opt::none, cds::sync::spin >::type  default_spin;

        // spin_32bit is cds::sync::reentrant_spin32
        typedef typename cds::opt::select_default< cds::opt::none, cds::sync::reentrant_spin32 >::type  spin_32bit;
        \endcode
    */
    template <typename Option, typename Default, typename Value = Option>
    struct select_default
    {
        typedef Value type ;   ///< metafunction result
    };
    //@cond
    template <typename Default>
    struct select_default< none, Default >
    {
        typedef Default type;
    };
    //@endcond

    /// Metafunction to select option value
    /**
        This metafunction is intended for extracting the value of the \p Option option.
        For example,
        \code
        #include <cds/opt/options.h>
        #include <type_traits> // only for testing purpose (static_assert)

        struct tag_a;

        // Define option
        typedef cds::opt::tag< tag_a >  tag_option;

        // What is the value of the tag_option?
        // How we can extract tag_a from tag_option?
        // Here is a solution:
        typedef cds::opt::value< tag_option >::tag  tag_option_value;

        // tag_option_value is the same as tag_a
        static_assert( std::is_same< tag_option_value, tag_a >::value, "Error: tag_option_value != tag_a" );

        \endcode
    */
    template <typename Option>
    struct value: public Option::template pack<none>
    {};


    /// [type-option] Option setter specifies a tag
    /**
        Suppose, you have a struct
        \code
        struct Feature
        {  .... };
        \endcode
        and you want that your class \p X would be derived from several \p Feature:
        \code
            class X: public Feature, public Feature
            { .... };
        \endcode

        How can you distinguish one \p Feature from another?
        You may use a tag option:
        \code
            template <typename Tag>
            struct Feature
            { .... };

            class tag_a;
            class tag_b;
            class X: public Feature< tag_a >, public Feature< tag_b >
            { .... };
        \endcode
        Now you can distinguish one \p Feature from another:
        \code
            X x;
            Feature<tag_a>& fa = static_cast< Feature<tag_a> >( x );
            Feature<tag_b>& fb = static_cast< Feature<tag_b> >( x );
        \endcode

        \p tag option setter allows you to do things like this for an option-centric approach:
        \code
        template <typename ...Options>
        struct Feature
        { .... };

        class tag_a;
        class tag_b;
        class X: public Feature< tag<tag_a> >, public Feature< tag<tag_b> >
        { .... };
        \endcode

        This option setter is widely used in cds::intrusive containers to distinguish
        between different intrusive part of container's node.

        An incomplete type can serve as a \p Tag.
    */
    template <typename Tag>
    struct tag {
        //@cond
        template<class Base> struct pack: public Base
        {
            typedef Tag tag;
        };
        //@endcond
    };

    /// [type-option] Option setter specifies lock class
    /**
        Specification of the \p Type class is:
        \code
        struct Lock {
            void lock();
            void unlock();
        };
        \endcode
    */
    template <typename Type>
    struct lock_type {
        //@cond
        template<class Base> struct pack: public Base
        {
            typedef Type lock_type;
        };
        //@endcond
    };

    /// [type-option] @ref cds_sync_monitor "Monitor" type setter
    /**
        This option setter specifyes @ref cds_sync_monitor "synchronization monitor"
        for blocking container.
    */
    template <typename Type>
    struct sync_monitor {
        //@cond
        template <class Base> struct pack : public Base
        {
            typedef Type sync_monitor;
        };
        //@endcond
    };

    /// [type-option] Back-off strategy option setter
    /**
        Back-off strategy used in some algorithm.
        See cds::backoff namespace for back-off explanation and supported interface.
    */
    template <typename Type>
    struct back_off {
        //@cond
        template <class Base> struct pack: public Base
        {
            typedef Type back_off;
        };
        //@endcond
    };

    /// [type-option] Option setter for garbage collecting schema used
    /**
        Possible values of \p GC template parameter are:
        - cds::gc::HP - Hazard Pointer garbage collector
        - cds::gc::DHP - Dynamic Hazard Pointer garbage collector
        - cds::gc::none::GC - No garbage collector (not supported for some containers)
    */
    template <typename GC>
    struct gc {
        //@cond
        template <class Base> struct pack: public Base
        {
            typedef GC gc;
        };
        //@endcond
    };

    /// [type-option] Option setter for an allocator
    /**
        \p Type is allocator with \p std::allocator interface. Default is value of macro CDS_DEFAULT_ALLOCATOR
        that, in turn, is \p std::allocator.

        The \p libcds containers actively use rebinding to convert an allocator of one type to another. Thus,
        you may specify any valid type as std::allocator's template parameter.

        See also opt::node_allocator
    */
    template <typename Type>
    struct allocator {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type allocator;
        };
        //@endcond
    };

    /// [type-option] Option setter for node allocator
    /**
        \p Type is allocator with \p std::allocator interface. Default is value of macro CDS_DEFAULT_ALLOCATOR
        that, in turn, is \p std::allocator.

        Many node-base containers require an allocator for maintaining data (container's node) and for internal use.
        Sometimes, this types of allocator should be different for performance reason.
        For example, we should like to allocate the node from a pool of preallocated nodes.
        Such pool can be seen as the node allocator.

        Usually, if a container supports \p opt::allocator and \p %opt::node_allocator options
        and \p opt::node_allocator is not specified the \p %opt::allocator option is used for maintaining the nodes.

        The \p libcds containers actively use rebinding to convert an allocator of one type to another. Thus,
        you may specify any valid type as std::allocator's template parameter.
    */
    template <typename Type>
    struct node_allocator {
        //@cond
            template <typename Base> struct pack: public Base
            {
                typedef Type node_allocator;
            };
        //@endcond
    };

    /// [type-option] Option setter for item counting
    /**
        Some data structure (for example, queues) has additional feature for item counting.
        This option allows to set up appropriate item counting policy for that data structure.

        Predefined option \p Type:
        - \p atomicity::empty_item_counter - no item counting performed. It is default policy for many
            containers
        - \p atomicity::item_counter - the class that provides atomic item counting
        - \p atomicity::cache_friendly_item_counter - cache-friendly atomic item counter
        - \p opt::v::sequential_item_counter - simple non-atomic item counter. This counter is not intended for
            concurrent containers and may be used only if it is explicitly noted.

        You may provide other implementation of \p atomicity::item_counter interface for your needs.

        Note, the item counting in lock-free containers cannot be exact; for example, if
        item counter for a container returns zero it is not mean that the container is empty.
        So, the item counter may be used for statistical purposes only.
    */
    template <typename Type>
    struct item_counter {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type item_counter;
        };
        //@endcond
    };

    /// Special alignment constants for \ref cds::opt::alignment option
    enum special_alignment {
        no_special_alignment = 0,   ///< no special alignment
        cache_line_alignment = 1    ///< use cache line size defined in cds/user_setup/cache_line.h
    };

    /// [value-option] Alignment option setter
    /**
        Alignment for some internal data of containers. May be useful to solve false sharing problem.
        \p Value defines desired alignment and it may be power of two integer or predefined values from
        \ref special_alignment enum.
    */
    template <unsigned int Value>
    struct alignment {
        //@cond
        template <typename Base> struct pack: public Base
        {
            enum { alignment = Value };
        };
        //@endcond
    };

    //@cond
    namespace details {
        template <typename Type, unsigned int Alignment>
        struct alignment_setter {
            typedef typename cds::details::aligned_type< Type, Alignment >::type  type;
        };

        template <typename Type>
        struct alignment_setter<Type, no_special_alignment> {
            typedef Type type;
        };

        template <typename Type>
        struct alignment_setter<Type, cache_line_alignment> {
            typedef typename cds::details::aligned_type< Type, c_nCacheLineSize >::type  type;
        };
    } // namespace details
    //@endcond

    /// Special padding constants for \p cds::opt::padding option
    enum special_padding {
        no_special_padding = 0,   ///< no special padding
        cache_line_padding = 1,   ///< use cache line size defined in cds/user_setup/cache_line.h

        /// Apply padding only for tiny data when data size is less than required padding
        /**
            The flag means that if your data size is less than the cacheline size, the padding is applyed.
            Otherwise no padding will be applyed.

            This flag is applyed for padding value:
            \code
            cds::opt::padding< cds::opt::cache_line_padding | cds::opt::padding_tiny_data_only >;
            cds::opt::padding< 256 | cds::opt::padding_tiny_data_only >;
            \endcode
        */
        padding_tiny_data_only = 0x80000000,

        //@cond
        padding_flags = padding_tiny_data_only
        //@endcond
    };

    /// [value-option] Padding option setter
    /**
        The padding for the internal data of some containers. May be useful to solve false sharing problem.
        \p Value defines desired padding and it may be power of two integer or predefined values from
        \p special_padding enum.
    */
    template <unsigned int Value>
    struct padding {
        //@cond
        template <typename Base> struct pack: public Base
        {
            enum { padding = Value };
        };
        //@endcond
    };

    //@cond
    template <unsigned Padding>
    struct actual_padding
    {
        enum { value = Padding & ~padding_flags };
    };

    template <>
    struct actual_padding<cache_line_padding>
    {
        enum { value = cds::c_nCacheLineSize };
    };

    template <>
    struct actual_padding<cache_line_padding| padding_tiny_data_only>
    {
        enum { value = cds::c_nCacheLineSize };
    };
    //@endcond

    //@cond
    namespace details {
        enum padding_vs_datasize {
            padding_datasize_less,
            padding_datasize_equal,
            padding_datasize_greater
        };

        template < typename T, unsigned int Padding, bool NoPadding, padding_vs_datasize Relation, bool TinyOnly >
        struct apply_padding_helper;

        template <typename T, padding_vs_datasize Relation, bool TinyOnly >
        struct apply_padding_helper < T, 0, true, Relation, TinyOnly >
        {
            struct type {
                T   data;
            };
            typedef void padding_type;
        };

        template <typename T, unsigned int Padding, bool TinyOnly >
        struct apply_padding_helper < T, Padding, false, padding_datasize_equal, TinyOnly >
        {
            struct type {
                T   data;
            };
            typedef void padding_type;
        };

        template <typename T, unsigned int Padding, bool TinyOnly >
        struct apply_padding_helper < T, Padding, false, padding_datasize_less, TinyOnly >
        {
            typedef uint8_t padding_type[Padding - sizeof( T )];
            struct type {
                T data;
                padding_type pad_;
            };

        };

        template <typename T, unsigned int Padding >
        struct apply_padding_helper < T, Padding, false, padding_datasize_greater, false >
        {
            typedef uint8_t padding_type[Padding - sizeof( T ) % Padding];
            struct type {
                T data;
                padding_type pad_;
            };
        };

        template <typename T, unsigned int Padding >
        struct apply_padding_helper < T, Padding, false, padding_datasize_greater, true >
        {
            struct type {
                T data;
            };
            typedef void padding_type;
        };

        template <typename T, unsigned int Padding >
        struct apply_padding
        {
        private:
            enum { padding = Padding & ~padding_flags };

        public:
            static constexpr const size_t c_nPadding =
                static_cast<unsigned int>(padding) == static_cast<unsigned int>(cache_line_padding) ? cds::c_nCacheLineSize :
                static_cast<unsigned int>(padding) == static_cast<unsigned int>(no_special_padding) ? 0 : padding;

            static_assert( (c_nPadding & (c_nPadding - 1)) == 0, "Padding must be a power-of-two number" );

            typedef apply_padding_helper< T,
                c_nPadding,
                c_nPadding == 0,
                sizeof( T ) < c_nPadding ? padding_datasize_less : sizeof( T ) == c_nPadding ? padding_datasize_equal : padding_datasize_greater,
                (Padding & padding_tiny_data_only) != 0
            > result;

            typedef typename result::type type;

            typedef typename std::conditional<
                std::is_same< typename result::padding_type, void >::value,
                unsigned int,
                typename result::padding_type
            >::type padding_type;
        };

    } // namespace details
    //@endcond


    /// [type-option] Generic option setter for statisitcs
    /**
        This option sets a type to gather statistics.
        The option is generic - no predefined type(s) is provided.
        The particular \p Type of statistics depends on internal structure of the object.
    */
    template <typename Type>
    struct stat {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type stat;
        };
        //@endcond
    };

    /// [type-option] Option setter for C++ memory model
    /**
        The <b>cds</b> library supports following memory ordering constraints for atomic operations in container implementation:
        - \p v::relaxed_ordering - relaxed C++ memory model. This mode supports full set of memory ordering constraints:
            \p memory_order_relaxed, \p memory_order_acquire, \p memory_order_release and so on.
        - \p v::sequential_consistent - sequentially consistent C++ memory model (default memory ordering for C++). In
            this mode any memory ordering constraint maps to \p memory_order_seq_cst.

        The \p Type template parameter can be \p v::relaxed_ordering or \p v::sequential_consistent.

        You may mix different memory ordering options for different containers: one declare as sequentially consistent,
        another declare as relaxed.
        Usually, \p v::relaxed_ordering is the default memory ordering for <b>libcds</b> containers.
    */
    template <typename Type>
    struct memory_model {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type memory_model;
        };
        //@endcond
    };

    /// [type-option] Base type traits option setter
    /**
        This option setter is intended generally for internal use for type rebinding.
    */
    template <typename Type>
    struct type_traits {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type type_traits;
        };
        //@endcond
    };

    /// Resizing policy option
    /**
        This option specifies the resizing policy that decides when to resize a container.
        Used in some containers, for example, in container::StripedHashSet, intrusive::StripedHashSet.

        The real resizing policy specified by \p Type does strongly depend on a container
        that supports this option, see container documentation about possibly \p Type values.
    */
    template <typename Type>
    struct resizing_policy {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type resizing_policy;
        };
        //@endcond
    };

    /// Copy policy option
    /**
        The copy policy defines an item copying algorithm which is used, for example, when a container is resized.
        It is very specific algorithm depending on type of the container.
    */
    template <typename Type>
    struct copy_policy {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type copy_policy;
        };
        //@endcond
    };

    /// Swap policy option
    /**
        The swap policy specifies an algorithm for swapping two objects.
        Usually, the default policy is \p std::swap (see opt::v::default_swap_policy):

        @code
        struct std_swap {
            template <typename T>
            void operator ()( T& v1, T& v2 )
            {
                std::swap( v1, v2 );
            }
        };
        @endcode
    */
    template <typename Type>
    struct swap_policy {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type swap_policy;
        };
        //@endcond
    };

    /// Move policy option
    /**
        The move policy specifies an algorithm for moving object content.
        In trivial case, it can be simple assignment.

        The move interface is:
        \code
        template <typename T>
        struct move_policy {
            void operator()( T& dest, T& src );
        };
        \endcode

        Note that in move algorithm the \p src source argument can be changed too.
        So you can use move semantics.

        Usually, the default move policy is opt::v::assignment_move_policy
    */
    template <typename Type>
    struct move_policy {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type move_policy;
        };
        //@endcond
    };

    /// [value-option] Enable sorting
    /**
        This option enables (<tt>Enable = true</tt>) or disables (<tt>Enable == false</tt>)
        sorting of a container.
    */
    template <bool Enable>
    struct sort {
        //@cond
        template <typename Base> struct pack: public Base
        {
            static bool const sort = Enable;
        };
        //@endcond
    };

    /// [type-option] Concurrent access policy
    /**
        This option specifies synchronization strategy for fine-grained lock-based containers.
        The option has no predefined \p Policy type.
        For each container that accepts this option the range of available \p Policy types
        is unique.
    */
    template <typename Policy>
    struct mutex_policy {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Policy mutex_policy;
        };
        //@endcond
    };


    /// [type-option] Random number generator
    /**
        The option specifies a random number generator.
        \p Random can be any STL random number generator producing
        unsigned integer: \p std::linear_congruential_engine,
        \p std::mersenne_twister_engine, \p std::subtract_with_carry_engine
        and so on, or \p opt::v::c_rand.

    */
    template <typename Random>
    struct random_engine {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Random random_engine;
        };
        //@endcond
    };

    /// [type-option] Free-list implementation
    /**
        See \p cds::intrusive::FreeList for free-list interface
    */
    template <typename FreeList>
    struct free_list {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef FreeList free_list;
        };
        //@endcond
    };

    //@cond
    // For internal use
    template <typename Accessor>
    struct key_accessor {
        template <typename Base> struct pack: public Base
        {
            typedef Accessor key_accessor;
        };
    };

    template <typename Traits, typename ReplaceWith, typename WhatReplace = none >
    struct replace_key_accessor {
        typedef typename std::conditional<
            std::is_same< typename Traits::key_accessor, WhatReplace >::value,
            typename opt::key_accessor< ReplaceWith >::template pack< Traits >,
            Traits
        >::type type;
    };
    //@endcond

}}  // namespace cds::opt


// ****************************************************
// Options predefined types and values

namespace cds { namespace opt {

    /// Predefined options value
    namespace v {

        /// Sequential non-atomic item counter
        /**
            This type of \p opt::item_counter option is not intended for concurrent containers
            and may be used only if it is explicitly noted.
        */
        class sequential_item_counter
        {
        public:
            typedef size_t counter_type    ;  ///< Counter type
        protected:
            counter_type  m_nCounter ;      ///< Counter

        public:
            sequential_item_counter()
                : m_nCounter(0)
            {}

            /// Returns current value of the counter
            counter_type    value() const
            {
                return m_nCounter;
            }

            /// Same as \ref value() with relaxed memory ordering
            operator counter_type() const
            {
                return value();
            }

            /// Increments the counter. Semantics: postincrement
            counter_type inc()
            {
                return m_nCounter++;
            }

            /// Decrements the counter. Semantics: postdecrement
            counter_type dec()
            {
                return m_nCounter--;
            }

            /// Preincrement
            counter_type operator ++()
            {
                return inc() + 1;
            }
            /// Postincrement
            counter_type operator ++(int)
            {
                return inc();
            }

            /// Predecrement
            counter_type operator --()
            {
                return dec() - 1;
            }
            /// Postdecrement
            counter_type operator --(int)
            {
                return dec();
            }

            /// Resets count to 0
            void reset()
            {
                m_nCounter = 0;
            }
        };

        /// Relaxed memory ordering \p opt::memory_model
        /**
            In this ordering the memory constraints are defined according to C++ Memory Model specification:
            each constraint is mapped to \p std::memory_order constraints one-to-one
        */
        struct relaxed_ordering {
            //@cond
            static const atomics::memory_order memory_order_relaxed    = atomics::memory_order_relaxed;
            static const atomics::memory_order memory_order_consume    = atomics::memory_order_consume;
            static const atomics::memory_order memory_order_acquire    = atomics::memory_order_acquire;
            static const atomics::memory_order memory_order_release    = atomics::memory_order_release;
            static const atomics::memory_order memory_order_acq_rel    = atomics::memory_order_acq_rel;
            static const atomics::memory_order memory_order_seq_cst    = atomics::memory_order_seq_cst;
            //@endcond
        };

        /// Sequential consistent \p opt::memory_memory ordering
        /**
            In this memory model any memory constraint is equivalent to \p std::memory_order_seq_cst.
        */
        struct sequential_consistent {
            //@cond
            static const atomics::memory_order memory_order_relaxed    = atomics::memory_order_seq_cst;
            static const atomics::memory_order memory_order_consume    = atomics::memory_order_seq_cst;
            static const atomics::memory_order memory_order_acquire    = atomics::memory_order_seq_cst;
            static const atomics::memory_order memory_order_release    = atomics::memory_order_seq_cst;
            static const atomics::memory_order memory_order_acq_rel    = atomics::memory_order_seq_cst;
            static const atomics::memory_order memory_order_seq_cst    = atomics::memory_order_seq_cst;
            //@endcond
        };

        //@cond
        /// Totally relaxed \p opt::memory_model ordering (do not use!)
        /**
            In this memory model any memory constraint is equivalent to \p std::memory_order_relaxed.
            @warning Do not use this model! It intended for testing purposes only
            to verify debugging instruments like Thread Sanitizer.
        */
        struct total_relaxed_ordering {
            static const atomics::memory_order memory_order_relaxed    = atomics::memory_order_relaxed;
            static const atomics::memory_order memory_order_consume    = atomics::memory_order_relaxed;
            static const atomics::memory_order memory_order_acquire    = atomics::memory_order_relaxed;
            static const atomics::memory_order memory_order_release    = atomics::memory_order_relaxed;
            static const atomics::memory_order memory_order_acq_rel    = atomics::memory_order_relaxed;
            static const atomics::memory_order memory_order_seq_cst    = atomics::memory_order_relaxed;
        };
        //@endcond


        /// Default swap policy for \p opt::swap_policy option
        /**
            The default swap policy is wrappr around \p std::swap algorithm.
        */
        struct default_swap_policy {
            /// Performs swapping of \p v1 and \p v2 using \p std::swap algo
            template <typename T>
            void operator()( T& v1, T& v2 ) const
            {
                std::swap( v1, v2 );
            }
        };

        /// \p opt::move_policy based on move-assignment operator
        struct assignment_move_policy
        {
            /// <tt> dest = std::move( src ) </tt>
            template <typename T>
            void operator()( T& dest, T&& src ) const
            {
                dest = std::move( src );
            }
        };

        /// \p rand() -base random number generator for \p opt::random_engine
        /**
            This generator returns a pseudorandom integer in the range 0 to \p RAND_MAX (32767).
        */
        struct c_rand {
            typedef unsigned int result_type; ///< Result type

            /// Constructor initializes object calling \p std::srand()
            c_rand()
            {
                std::srand(1);
            }

            /// Returns next random number calling \p std::rand()
            result_type operator()()
            {
                return (result_type) std::rand();
            }
        };
    } // namespace v

}} // namespace cds::opt


// ****************************************************
// Options metafunctions

namespace cds { namespace opt {

    //@cond
    namespace details {
        template <typename OptionList, typename Option>
        struct do_pack
        {
            // Use "pack" member template to pack options
            typedef typename Option::template pack<OptionList> type;
        };

        template <typename ...T> class typelist;

        template <typename Typelist> struct typelist_head;
        template <typename Head, typename ...Tail>
        struct typelist_head< typelist<Head, Tail...> > {
            typedef Head type;
        };
        template <typename Head>
        struct typelist_head< typelist<Head> > {
            typedef Head type;
        };

        template <typename Typelist> struct typelist_tail;
        template <typename Head, typename ...Tail>
        struct typelist_tail< typelist<Head, Tail...> > {
            typedef typelist<Tail...> type;
        };
        template <typename Head>
        struct typelist_tail< typelist<Head> > {
            typedef typelist<> type;
        };

        template <typename OptionList, typename Typelist>
        struct make_options_impl {
            typedef typename make_options_impl<
                typename do_pack<
                    OptionList,
                    typename typelist_head< Typelist >::type
                >::type,
                typename typelist_tail<Typelist>::type
            >::type type;
        };

        template <typename OptionList>
        struct make_options_impl<OptionList, typelist<> > {
            typedef OptionList type;
        };
    }   // namespace details
    //@endcond

    /// make_options metafunction
    /** @headerfile cds/opt/options.h

        The metafunction converts option list \p Options to traits structure.
        The result of metafunction is \p type.

        Template parameter \p OptionList is default option set (default traits).
        \p Options is option list.
    */
    template <typename OptionList, typename... Options>
    struct make_options {
#ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined type ;   ///< Result of the metafunction
#else
        typedef typename details::make_options_impl< OptionList, details::typelist<Options...> >::type type;
#endif
    };


    // *****************************************************************
    // find_type_traits metafunction
    // *****************************************************************

    //@cond
    namespace details {
        template <typename... Options>
        struct find_type_traits_option;

        template <>
        struct find_type_traits_option<> {
            typedef cds::opt::none  type;
        };

        template <typename Any>
        struct find_type_traits_option< Any > {
            typedef cds::opt::none type;
        };

        template <typename Any>
        struct find_type_traits_option< cds::opt::type_traits< Any > > {
            typedef Any type;
        };

        template <typename Any, typename... Options>
        struct find_type_traits_option< cds::opt::type_traits< Any >, Options... > {
            typedef Any type;
        };

        template <typename Any, typename... Options>
        struct find_type_traits_option< Any, Options... > {
            typedef typename find_type_traits_option< Options... >::type type;
        };
    } // namespace details
    //@endcond

    /// Metafunction to find opt::type_traits option in \p Options list
    /** @headerfile cds/opt/options.h

        If \p Options contains \p opt::type_traits option then it is the metafunction result.
        Otherwise the result is \p DefaultOptons.
    */
    template <typename DefaultOptions, typename... Options>
    struct find_type_traits {
        typedef typename select_default< typename details::find_type_traits_option<Options...>::type, DefaultOptions>::type type ;  ///< Metafunction result
    };


    // *****************************************************************
    // find_option metafunction
    // *****************************************************************

    //@cond
    namespace details {
        template <typename What, typename... Options>
        struct find_option;

        struct compare_ok;
        struct compare_fail;

        template <typename A, typename B>
        struct compare_option
        {
            typedef compare_fail type;
        };

        template <template <typename> class Opt, typename A, typename B>
        struct compare_option< Opt<A>, Opt<B> >
        {
            typedef compare_ok   type;
        };

        // Specializations for integral type of option
#define CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_( _type ) template <template <_type> class What, _type A, _type B> \
        struct compare_option< What<A>, What<B> > { typedef compare_ok type ; };

        // For user-defined enum types
#define CDS_DECLARE_FIND_OPTION_INTEGRAL_SPECIALIZATION( _type ) namespace cds { namespace opt { namespace details { CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(_type ) }}}

        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(bool)
        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(char)
        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(unsigned char)
        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(signed char)
        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(short int)
        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(unsigned short int)
        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(int)
        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(unsigned int)
        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(long)
        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(unsigned long)
        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(long long)
        CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION_(unsigned long long)


        template <typename CompResult, typename Ok, typename Fail>
        struct select_option
        {
            typedef Fail    type;
        };

        template <typename Ok, typename Fail>
        struct select_option< compare_ok, Ok, Fail >
        {
            typedef Ok      type;
        };

        template <typename What>
        struct find_option< What > {
            typedef What    type;
        };

        template <typename What, typename Opt>
        struct find_option< What, Opt > {
            typedef typename select_option<
                typename compare_option< What, Opt >::type
                ,Opt
                ,What
            >::type type;
        };

        template <typename What, typename Opt, typename... Options>
        struct find_option< What, Opt, Options... > {
            typedef typename select_option<
                typename compare_option< What, Opt >::type
                ,Opt
                ,typename find_option< What, Options... >::type
            >::type type;
        };
    } // namespace details
    //@endcond

    /// Metafunction to find \p What option in \p Options list
    /** @headerfile cds/opt/options.h

        If \p Options contains \p What< Val > option for any \p Val then the result is \p What< Val >
        Otherwise the result is \p What.

        Example:
        \code
        #include <cds/opt/options.h>
        namespace co = cds::opt;

        struct default_tag;
        struct tag_a;
        struct tag_b;

        // Find option co::tag.

        // res1 is co::tag< tag_a >
        typedef co::find_option< co::tag< default_tag >, co::gc< cds::gc::HP >, co::tag< tag_a > >::type res1;

        // res2 is default co::tag< default_tag >
        typedef co::find_option< co::tag< default_tag >, co::less< x >, co::hash< H > >::type res2;

        // Multiple option co::tag. The first option is selected
        // res3 is default co::tag< tag_a >
        typedef co::find_option< co::tag< default_tag >, co::tag< tag_a >, co::tag< tag_b > >::type res3;

        \endcode
    */
    template <typename What, typename... Options>
    struct find_option {
        typedef typename details::find_option<What, Options...>::type   type ;  ///< Metafunction result
    };


    // *****************************************************************
    // select metafunction
    // *****************************************************************

    //@cond
    namespace details {

        template <typename What, typename... Pairs>
        struct select;

        template <typename What, typename Value>
        struct select< What, What, Value>
        {
            typedef Value   type;
        };

        template <typename What, typename Tag, typename Value>
        struct select<What, Tag, Value>
        {
            typedef What    type;
        };

        template <typename What, typename Value, typename... Pairs>
        struct select< What, What, Value, Pairs...>
        {
            typedef Value   type;
        };

        template <typename What, typename Tag, typename Value, typename... Pairs>
        struct select< What, Tag, Value, Pairs...>
        {
            typedef typename select<What, Pairs...>::type   type;
        };
    }   // namespace details
    //@endcond

    /// Select option metafunction
    /** @headerfile cds/opt/options.h

        Pseudocode:
        \code
        select <What, T1, R1, T2, R2, ... Tn, Rn> ::=
            if What == T1 then return R1
            if What == T2 then return R2
            ...
            if What == Tn then return Rn
            else return What
        \endcode
    */
    template <typename What, typename... Pairs>
    struct select {
        typedef typename details::select< What, Pairs...>::type  type    ;   ///< Metafunction result
    };

}}  // namespace cds::opt


#endif  // #ifndef CDSLIB_OPT_OPTIONS_H
