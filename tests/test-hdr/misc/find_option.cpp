/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include <type_traits>
#include <cds/opt/options.h>

// Value options
namespace {

    template <int Val>
    struct int_opt {
        static const int value = Val;
    };

    template <bool Val>
    struct bool_opt {
        static const bool value = Val;
    };

    enum user_enum {
        val_zero, val_one, val_two, val_three, val_four, val_five
    };

    template <user_enum Val>
    struct enum_opt {
        static const user_enum value = Val;
    };
}

// Declare necessary cds::opt::find_option specialization for user-provided enum type
CDS_DECLARE_FIND_OPTION_INTEGRAL_SPECIALIZATION( user_enum )

#if CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION < 40500
// GCC 4.4 does not support local struct declarations
namespace {
    struct tag_default;
    struct tag_a;
    struct tag_b;
}
#endif

void find_option_compiler_test()
{

    // *************************************************
    // Type options
    //
#if !(CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION < 40500)
    // GCC 4.4 does not support local struct declarations
    struct tag_default;
    struct tag_a;
    struct tag_b;
#endif

    // Option not found
    static_assert( (std::is_same<
        cds::opt::find_option< cds::opt::tag<tag_default>, cds::opt::stat<tag_a>, bool_opt<false> >::type,
        cds::opt::tag<tag_default>
    >::value), "Result != tag_default" );

    // Option found once
    static_assert( (std::is_same<
        cds::opt::find_option< cds::opt::tag<tag_default>, cds::opt::tag<tag_a> >::type,
        cds::opt::tag<tag_a>
    >::value), "Result != tag_a" );

    static_assert( (std::is_same<
        cds::opt::find_option< cds::opt::tag<tag_default>, cds::opt::stat<tag_a>, cds::opt::tag<tag_a> >::type,
        cds::opt::tag<tag_a>
    >::value), "Result != tag_a" );

    // First option
    static_assert( (std::is_same<
        cds::opt::find_option< cds::opt::tag<tag_default>
        ,cds::opt::tag<tag_a>   // desired
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        >::type,
        cds::opt::tag<tag_a>
    >::value), "Result != tag_a" );

    // Last option
    static_assert( (std::is_same<
        cds::opt::find_option< cds::opt::tag<tag_default>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::tag<tag_a>   // desired
        >::type,
        cds::opt::tag<tag_a>
    >::value), "Result != tag_a" );

    // Middle option
    static_assert( (std::is_same<
        cds::opt::find_option< cds::opt::tag<tag_default>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::tag<tag_a>   // desired
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        >::type,
        cds::opt::tag<tag_a>
    >::value), "Result != tag_a" );

    // Option not found
    static_assert( (std::is_same<
        cds::opt::find_option< cds::opt::tag<tag_default>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_default>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        >::type,
        cds::opt::tag<tag_default>
    >::value), "Result != tag_default" );

    // Multiple options
    static_assert( (std::is_same<
        cds::opt::find_option< cds::opt::tag<tag_default>, cds::opt::tag<tag_a>, cds::opt::tag<tag_b> >::type,
        cds::opt::tag<tag_a>
    >::value), "Result != tag_a" );

    static_assert( (std::is_same<
        cds::opt::find_option< cds::opt::tag<tag_default>
        ,cds::opt::tag<tag_a>   // desired - first accepted
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_b>
        ,cds::opt::stat<tag_a>
        ,cds::opt::stat<tag_a>
        ,cds::opt::tag<tag_b>    // desired
        >::type,
        cds::opt::tag<tag_a>
    >::value), "Result != tag_a" );



    // *****************************************************
    // Value options

    // Not found
    static_assert( (std::is_same<
        cds::opt::find_option< int_opt<15>, bool_opt<false>, cds::opt::stat<tag_a> >::type,
        int_opt<15>
    >::value), "Result != int_opt<15>" );

    static_assert( (std::is_same<
        cds::opt::find_option< int_opt<15>, int_opt<100>, cds::opt::stat<tag_a> >::type,
        int_opt<100>
    >::value), "Result != int_opt<100>" );

    static_assert( (std::is_same<
        cds::opt::find_option< int_opt<15>, int_opt<100>, cds::opt::stat<tag_a>, bool_opt<true>, int_opt<200> >::type,
        int_opt<100>
    >::value), "Result != int_opt<100>" );

    // User-provided enum type
    static_assert( (std::is_same<
        cds::opt::find_option< enum_opt<val_zero>, int_opt<100>, cds::opt::stat<tag_a>, int_opt<200> >::type,
        enum_opt<val_zero>
    >::value), "Result != enum_opt<val_zero>" );

    static_assert( (std::is_same<
        cds::opt::find_option< enum_opt<val_zero>, int_opt<100>, cds::opt::stat<tag_a>, enum_opt<val_three>, int_opt<200> >::type,
        enum_opt<val_three>
    >::value), "Result != enum_opt<val_three>" );

}

void test_extracting_option_value()
{
#if !(CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION < 40500)
    // GCC 4.4 does not support local struct declarations
    struct tag_a;
#endif
    // Define option
    typedef cds::opt::tag< tag_a >  tag_option;

    // What is the value of the tag_option?
    // How we can extract tag_a from tag_option?
    // Here is a solution:
    typedef cds::opt::value< tag_option >::tag  tag_option_value;

    // tag_option_value is the same as tag_a
    static_assert( (std::is_same< tag_option_value, tag_a >::value), "Error getting the value of option: tag_option_value != tag_a" );

    // Value-option
    typedef cds::opt::alignment< 16 >   align_option;
    static_assert( cds::opt::value< align_option >::alignment == 16, "Error getting the value of option: option value != 16" );
}
