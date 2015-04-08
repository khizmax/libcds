//$$CDS-header$$

#ifndef CDSLIB_OPT_OPTIONS_H
#define CDSLIB_OPT_OPTIONS_H

/*
    Framework to define template options

    Editions:
        2011.01.23 khizmax  Created
*/

#include <cds/details/aligned_type.h>
#include <cds/user_setup/allocator.h>
#include <cds/user_setup/cache_line.h>
#include <cds/algo/atomic.h>
#include <stdlib.h> // rand, srand

namespace cds {

/// Framework to define template options
/**
    There are two kind of options:
    - \p type-option - option that determines a data type. The template argument \p Type of the option is a type.
    - \p value-option - option that determines a value. The template argument \p Value of the option is a value.
*/
namespace opt {

    /// Predefined options value (generally, for the options that determine the data types)
    namespace v {}

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
        - atomicity::empty_item_counter - no item counting performed. It is default policy for many
            containers
        - atomicity::item_counter - the class that provides atomically item counting
        - opt::v::sequential_item_counter - simple non-atomic item counter. This item counter is not intended for
            concurrent containers and may be used only if it is explicitly noted.

        You may provide other implementation of atomicity::item_counter interface for your needs.

        Note, the item counting in lock-free containers cannot be exact; for example, if
        item counter for a container returns zero it is not mean that the container is empty.
        Thus, item counter may be used for statistical purposes only.
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

    namespace v {
        /// Sequential non-atomic item counter
        /**
            This type of item counter is not intended for concurrent containers
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
    } // namespace v

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

        /// Apply padding only for tiny data of size less than required padding
        /**
            The flag means that if your data size is less than the casheline size, the padding is applyed.
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
        };

        template <typename T, unsigned int Padding, bool TinyOnly >
        struct apply_padding_helper < T, Padding, false, padding_datasize_equal, TinyOnly >
        {
            struct type {
                T   data;
            };
        };

        template <typename T, unsigned int Padding, bool TinyOnly >
        struct apply_padding_helper < T, Padding, false, padding_datasize_less, TinyOnly >
        {
            struct type {
                T data;
                uint8_t pad_[Padding - sizeof( T )];
            };
        };

        template <typename T, unsigned int Padding >
        struct apply_padding_helper < T, Padding, false, padding_datasize_greater, false >
        {
            struct type {
                T data;
                uint8_t pad_[Padding - sizeof( T ) % Padding];
            };
        };

        template <typename T, unsigned int Padding >
        struct apply_padding_helper < T, Padding, false, padding_datasize_greater, true >
        {
            struct type {
                T data;
            };
        };

        template <typename T, unsigned int Padding >
        struct apply_padding
        {
        private:
            enum { padding = Padding & ~padding_flags };

        public:
            static CDS_CONSTEXPR const size_t c_nPadding =
                static_cast<unsigned int>(padding) == static_cast<unsigned int>(cache_line_padding) ? cds::c_nCacheLineSize :
                static_cast<unsigned int>(padding) == static_cast<unsigned int>(no_special_padding) ? 0 : padding;

            static_assert( (c_nPadding & (c_nPadding - 1)) == 0, "Padding must be a power-of-two number" );

            typedef typename apply_padding_helper< T,
                c_nPadding,
                c_nPadding == 0,
                sizeof( T ) < c_nPadding ? padding_datasize_less : sizeof( T ) == c_nPadding ? padding_datasize_equal : padding_datasize_greater,
                (Padding & padding_tiny_data_only) != 0
            >::type type;
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
        - v::relaxed_ordering - relaxed C++ memory model. This mode supports full set of memory ordering constraints:
            \p memory_order_relaxed, \p memory_order_acquire, \p memory_order_release and so on.
        - v::sequential_consistent - sequentially consistent C++ memory model (default memory ordering for C++). In
            this mode any memory ordering constraint maps to \p memory_order_seq_cst.

        The \p Type template parameter can be v::relaxed_ordering or v::sequential_consistent.

        You may mix different memory ordering options for different containers: one declare as sequentially consistent,
        another declare as relaxed.
        Usually, v::relaxed_ordering is the default memory ordering for <b>cds</b> containers.
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

    namespace v {
        /// Relaxed memory ordering model
        /**
            In this memory model the memory constraints are defined according to C++ Memory Model specification.

            See opt::memory_model for explanations
        */
        struct relaxed_ordering {
            //@cond

            // For new C++11 (cds-1.1.0)
            static const atomics::memory_order memory_order_relaxed    = atomics::memory_order_relaxed;
            static const atomics::memory_order memory_order_consume    = atomics::memory_order_consume;
            static const atomics::memory_order memory_order_acquire    = atomics::memory_order_acquire;
            static const atomics::memory_order memory_order_release    = atomics::memory_order_release;
            static const atomics::memory_order memory_order_acq_rel    = atomics::memory_order_acq_rel;
            static const atomics::memory_order memory_order_seq_cst    = atomics::memory_order_seq_cst;
            //@endcond
        };

        /// Sequential consistent memory ordering model
        /**
            In this memory model any memory constraint is equivalent to \p memory_order_seq_cst.

            See opt::memory_model for explanations
        */
        struct sequential_consistent {
            //@cond

            // For new C++11 (cds-1.1.0)
            static const atomics::memory_order memory_order_relaxed    = atomics::memory_order_seq_cst;
            static const atomics::memory_order memory_order_consume    = atomics::memory_order_seq_cst;
            static const atomics::memory_order memory_order_acquire    = atomics::memory_order_seq_cst;
            static const atomics::memory_order memory_order_release    = atomics::memory_order_seq_cst;
            static const atomics::memory_order memory_order_acq_rel    = atomics::memory_order_seq_cst;
            static const atomics::memory_order memory_order_seq_cst    = atomics::memory_order_seq_cst;
            //@endcond
        };
    } // namespace v

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

    namespace v {

        /// Default swap policy (see opt::swap_policy option)
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
    } // namespace v

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

    namespace v {
        /// \ref opt::move_policy "Move policy" based on assignment operator
        struct assignment_move_policy
        {
            /// <tt> dest = src </tt>
            template <typename T>
            void operator()( T& dest, T const& src ) const
            {
                dest = src;
            }
        };
    } // namespace v

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
        and so on, or opt::v::c_rand.

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

    namespace v {
        /// \p rand() -base random number generator
        /**
            This generator returns a pseudorandom integer in the range 0 to \p RAND_MAX (32767).
        */
        struct c_rand {
            typedef unsigned int result_type; ///< Result type

            /// Constructor initializes object calling \p srand()
            c_rand()
            {
                srand(1);
            }

            /// Returns next random number calling \p rand()
            result_type operator()()
            {
                return (result_type) rand();
            }
        };
    } // namespace v

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

#include <cds/opt/make_options_var.h>

#endif  // #ifndef CDSLIB_OPT_OPTIONS_H
