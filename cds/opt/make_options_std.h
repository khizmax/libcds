//$$CDS-header$$

#ifndef __CDS_OPT_MAKE_OPTIONS_STD_H
#define __CDS_OPT_MAKE_OPTIONS_STD_H

#ifndef __CDS_OPT_OPTIONS_H
#   error <cds/opt/options.h> must be included instead of <cds/opt/make_options_std.h>
#endif

#include <cds/details/void_selector.h>

// @cond

#define CDS_DECL_OPTIONS1   typename O1=cds::opt::none
#define CDS_DECL_OPTIONS2   CDS_DECL_OPTIONS1,typename O2=cds::opt::none
#define CDS_DECL_OPTIONS3   CDS_DECL_OPTIONS2,typename O3=cds::opt::none
#define CDS_DECL_OPTIONS4   CDS_DECL_OPTIONS3,typename O4=cds::opt::none
#define CDS_DECL_OPTIONS5   CDS_DECL_OPTIONS4,typename O5=cds::opt::none
#define CDS_DECL_OPTIONS6   CDS_DECL_OPTIONS5,typename O6=cds::opt::none
#define CDS_DECL_OPTIONS7   CDS_DECL_OPTIONS6,typename O7=cds::opt::none
#define CDS_DECL_OPTIONS8   CDS_DECL_OPTIONS7,typename O8=cds::opt::none
#define CDS_DECL_OPTIONS9   CDS_DECL_OPTIONS8,typename O9=cds::opt::none
#define CDS_DECL_OPTIONS10  CDS_DECL_OPTIONS9,typename O10=cds::opt::none
#define CDS_DECL_OPTIONS11  CDS_DECL_OPTIONS10,typename O11=cds::opt::none
#define CDS_DECL_OPTIONS12  CDS_DECL_OPTIONS11,typename O12=cds::opt::none
#define CDS_DECL_OPTIONS13  CDS_DECL_OPTIONS12,typename O13=cds::opt::none
#define CDS_DECL_OPTIONS14  CDS_DECL_OPTIONS13,typename O14=cds::opt::none
#define CDS_DECL_OPTIONS15  CDS_DECL_OPTIONS14,typename O15=cds::opt::none
#define CDS_DECL_OPTIONS16  CDS_DECL_OPTIONS15,typename O16=cds::opt::none

#define CDS_DECL_OPTIONS    CDS_DECL_OPTIONS16

#define CDS_DECL_OTHER_OPTIONS1   typename OO1=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS2   CDS_DECL_OTHER_OPTIONS1,typename OO2=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS3   CDS_DECL_OTHER_OPTIONS2,typename OO3=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS4   CDS_DECL_OTHER_OPTIONS3,typename OO4=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS5   CDS_DECL_OTHER_OPTIONS4,typename OO5=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS6   CDS_DECL_OTHER_OPTIONS5,typename OO6=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS7   CDS_DECL_OTHER_OPTIONS6,typename OO7=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS8   CDS_DECL_OTHER_OPTIONS7,typename OO8=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS9   CDS_DECL_OTHER_OPTIONS8,typename OO9=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS10  CDS_DECL_OTHER_OPTIONS9,typename OO10=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS11  CDS_DECL_OTHER_OPTIONS10,typename OO11=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS12  CDS_DECL_OTHER_OPTIONS11,typename OO12=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS13  CDS_DECL_OTHER_OPTIONS12,typename OO13=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS14  CDS_DECL_OTHER_OPTIONS13,typename OO14=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS15  CDS_DECL_OTHER_OPTIONS14,typename OO15=cds::opt::none
#define CDS_DECL_OTHER_OPTIONS16  CDS_DECL_OTHER_OPTIONS15,typename OO16=cds::opt::none

// for template specializations
#define CDS_SPEC_OPTIONS1   typename O1
#define CDS_SPEC_OPTIONS2   CDS_SPEC_OPTIONS1,typename O2
#define CDS_SPEC_OPTIONS3   CDS_SPEC_OPTIONS2,typename O3
#define CDS_SPEC_OPTIONS4   CDS_SPEC_OPTIONS3,typename O4
#define CDS_SPEC_OPTIONS5   CDS_SPEC_OPTIONS4,typename O5
#define CDS_SPEC_OPTIONS6   CDS_SPEC_OPTIONS5,typename O6
#define CDS_SPEC_OPTIONS7   CDS_SPEC_OPTIONS6,typename O7
#define CDS_SPEC_OPTIONS8   CDS_SPEC_OPTIONS7,typename O8
#define CDS_SPEC_OPTIONS9   CDS_SPEC_OPTIONS8,typename O9
#define CDS_SPEC_OPTIONS10  CDS_SPEC_OPTIONS9,typename O10
#define CDS_SPEC_OPTIONS11  CDS_SPEC_OPTIONS10,typename O11
#define CDS_SPEC_OPTIONS12  CDS_SPEC_OPTIONS11,typename O12
#define CDS_SPEC_OPTIONS13  CDS_SPEC_OPTIONS12,typename O13
#define CDS_SPEC_OPTIONS14  CDS_SPEC_OPTIONS13,typename O14
#define CDS_SPEC_OPTIONS15  CDS_SPEC_OPTIONS14,typename O15
#define CDS_SPEC_OPTIONS16  CDS_SPEC_OPTIONS15,typename O16

#define CDS_SPEC_OPTIONS    CDS_SPEC_OPTIONS16

#define CDS_OPTIONS1    O1
#define CDS_OPTIONS2    O1,O2
#define CDS_OPTIONS3    O1,O2,O3
#define CDS_OPTIONS4    O1,O2,O3,O4
#define CDS_OPTIONS5    O1,O2,O3,O4,O5
#define CDS_OPTIONS6    O1,O2,O3,O4,O5,O6
#define CDS_OPTIONS7    O1,O2,O3,O4,O5,O6,O7
#define CDS_OPTIONS8    O1,O2,O3,O4,O5,O6,O7,O8
#define CDS_OPTIONS9    O1,O2,O3,O4,O5,O6,O7,O8,O9
#define CDS_OPTIONS10   O1,O2,O3,O4,O5,O6,O7,O8,O9,O10
#define CDS_OPTIONS11   O1,O2,O3,O4,O5,O6,O7,O8,O9,O10,O11
#define CDS_OPTIONS12   O1,O2,O3,O4,O5,O6,O7,O8,O9,O10,O11,O12
#define CDS_OPTIONS13   O1,O2,O3,O4,O5,O6,O7,O8,O9,O10,O11,O12,O13
#define CDS_OPTIONS14   O1,O2,O3,O4,O5,O6,O7,O8,O9,O10,O11,O12,O13,O14
#define CDS_OPTIONS15   O1,O2,O3,O4,O5,O6,O7,O8,O9,O10,O11,O12,O13,O14,O15
#define CDS_OPTIONS16   O1,O2,O3,O4,O5,O6,O7,O8,O9,O10,O11,O12,O13,O14,O15,O16
//#define CDS_OPTIONS17   O1,O2,O3,O4,O5,O6,O7,O8,O9,O10,O11,O12,O13,O14,O15,O16,O17
//#define CDS_OPTIONS18   O1,O2,O3,O4,O5,O6,O7,O8,O9,O10,O11,O12,O13,O14,O15,O16,O17,O18
//#define CDS_OPTIONS19   O1,O2,O3,O4,O5,O6,O7,O8,O9,O10,O11,O12,O13,O14,O15,O16,O17,O18,O19
//#define CDS_OPTIONS20   O1,O2,O3,O4,O5,O6,O7,O8,O9,O10,O11,O12,O13,O14,O15,O16,O17,O18,O19,O20

#define CDS_OPTIONS     CDS_OPTIONS16

#define CDS_OTHER_OPTIONS1    OO1
#define CDS_OTHER_OPTIONS2    OO1,OO2
#define CDS_OTHER_OPTIONS3    OO1,OO2,OO3
#define CDS_OTHER_OPTIONS4    OO1,OO2,OO3,OO4
#define CDS_OTHER_OPTIONS5    OO1,OO2,OO3,OO4,OO5
#define CDS_OTHER_OPTIONS6    OO1,OO2,OO3,OO4,OO5,OO6
#define CDS_OTHER_OPTIONS7    OO1,OO2,OO3,OO4,OO5,OO6,OO7
#define CDS_OTHER_OPTIONS8    OO1,OO2,OO3,OO4,OO5,OO6,OO7,OO8
#define CDS_OTHER_OPTIONS9    OO1,OO2,OO3,OO4,OO5,OO6,OO7,OO8,OO9
#define CDS_OTHER_OPTIONS10   OO1,OO2,OO3,OO4,OO5,OO6,OO7,OO8,OO9,OO10
#define CDS_OTHER_OPTIONS11   OO1,OO2,OO3,OO4,OO5,OO6,OO7,OO8,OO9,OO10,OO11
#define CDS_OTHER_OPTIONS12   OO1,OO2,OO3,OO4,OO5,OO6,OO7,OO8,OO9,OO10,OO11,OO12
#define CDS_OTHER_OPTIONS13   OO1,OO2,OO3,OO4,OO5,OO6,OO7,OO8,OO9,OO10,OO11,OO12,OO13
#define CDS_OTHER_OPTIONS14   OO1,OO2,OO3,OO4,OO5,OO6,OO7,OO8,OO9,OO10,OO11,OO12,OO13,OO14
#define CDS_OTHER_OPTIONS15   OO1,OO2,OO3,OO4,OO5,OO6,OO7,OO8,OO9,OO10,OO11,OO12,OO13,OO14,OO15
#define CDS_OTHER_OPTIONS16   OO1,OO2,OO3,OO4,OO5,OO6,OO7,OO8,OO9,OO10,OO11,OO12,OO13,OO14,OO15,OO16

namespace cds { namespace opt {

    template<typename OptionList, typename Option>
    struct do_pack
    {
        // Use "pack" member template to pack options
        typedef typename Option::template pack<OptionList> type;
    };

    template <
        typename DefaultOptions
        ,typename O1 = none
        ,typename O2 = none
        ,typename O3 = none
        ,typename O4 = none
        ,typename O5 = none
        ,typename O6 = none
        ,typename O7 = none
        ,typename O8 = none
        ,typename O9 = none
        ,typename O10 = none
        ,typename O11 = none
        ,typename O12 = none
        ,typename O13 = none
        ,typename O14 = none
        ,typename O15 = none
        ,typename O16 = none
    >
    struct make_options {
        /*
        typedef
            typename do_pack<
                typename do_pack<
                    typename do_pack<
                        typename do_pack<
                            typename do_pack<
                                typename do_pack<
                                    typename do_pack<
                                        typename do_pack<
                                            typename do_pack<
                                                typename do_pack<
                                                    typename do_pack<
                                                        typename do_pack<
                                                            typename do_pack<
                                                                typename do_pack<
                                                                    typename do_pack<
                                                                        typename do_pack<
                                                                            DefaultOptions
                                                                            ,O16
                                                                        >::type
                                                                        ,O15
                                                                    >::type
                                                                    ,O14
                                                                >::type
                                                                ,O13
                                                            >::type
                                                            ,O12
                                                        >::type
                                                        ,O11
                                                    >::type
                                                    ,O10
                                                >::type
                                                ,O9
                                            >::type
                                            ,O8
                                        >::type
                                        ,O7
                                    >::type
                                    ,O6
                                >::type
                                ,O5
                            >::type
                            ,O4
                        >::type
                        ,O3
                    >::type
                    ,O2
                >::type
                ,O1
            >::type
        type;
        */
        struct type: public
            do_pack<
                typename do_pack<
                    typename do_pack<
                        typename do_pack<
                            typename do_pack<
                                typename do_pack<
                                    typename do_pack<
                                        typename do_pack<
                                            typename do_pack<
                                                typename do_pack<
                                                    typename do_pack<
                                                        typename do_pack<
                                                            typename do_pack<
                                                                typename do_pack<
                                                                    typename do_pack<
                                                                        typename do_pack<
                                                                            DefaultOptions
                                                                            ,O16
                                                                        >::type
                                                                        ,O15
                                                                    >::type
                                                                    ,O14
                                                                >::type
                                                                ,O13
                                                            >::type
                                                            ,O12
                                                        >::type
                                                        ,O11
                                                    >::type
                                                    ,O10
                                                >::type
                                                ,O9
                                            >::type
                                            ,O8
                                        >::type
                                        ,O7
                                    >::type
                                    ,O6
                                >::type
                                ,O5
                            >::type
                            ,O4
                        >::type
                        ,O3
                    >::type
                    ,O2
                >::type
                ,O1
            >::type
        {};
    };


    // *****************************************************************
    // find_type_traits metafunction
    // *****************************************************************

    namespace details {
        template <typename T, typename DefaultOptions>
        struct find_type_traits_option {
            typedef DefaultOptions type;
        };

        template <typename T, typename DefaultOptions>
        struct find_type_traits_option< cds::opt::type_traits<T>, DefaultOptions> {
            typedef T  type;
        };
    }

    template <
        typename DefaultOptions
        ,typename O1 = none
        ,typename O2 = none
        ,typename O3 = none
        ,typename O4 = none
        ,typename O5 = none
        ,typename O6 = none
        ,typename O7 = none
        ,typename O8 = none
        ,typename O9 = none
        ,typename O10 = none
        ,typename O11 = none
        ,typename O12 = none
        ,typename O13 = none
        ,typename O14 = none
        ,typename O15 = none
        ,typename O16 = none
    >
    struct find_type_traits {
        /*
        typedef typename details::find_type_traits_option< O1,
            typename details::find_type_traits_option< O2,
                typename details::find_type_traits_option< O3,
                    typename details::find_type_traits_option< O4,
                        typename details::find_type_traits_option< O5,
                            typename details::find_type_traits_option< O6,
                                typename details::find_type_traits_option< O7,
                                    typename details::find_type_traits_option< O8,
                                        typename details::find_type_traits_option< O9,
                                            typename details::find_type_traits_option< O10,
                                                typename details::find_type_traits_option< O11,
                                                    typename details::find_type_traits_option< O12,
                                                        typename details::find_type_traits_option< O13,
                                                            typename details::find_type_traits_option< O14,
                                                                typename details::find_type_traits_option< O15,
                                                                    typename details::find_type_traits_option< O16, DefaultOptions>::type
                                                                >::type
                                                            >::type
                                                        >::type
                                                    >::type
                                                >::type
                                            >::type
                                        >::type
                                    >::type
                                >::type
                            >::type
                        >::type
                    >::type
                >::type
            >::type
        >::type type;
        */
        struct type: public details::find_type_traits_option< O1,
            typename details::find_type_traits_option< O2,
                typename details::find_type_traits_option< O3,
                    typename details::find_type_traits_option< O4,
                        typename details::find_type_traits_option< O5,
                            typename details::find_type_traits_option< O6,
                                typename details::find_type_traits_option< O7,
                                    typename details::find_type_traits_option< O8,
                                        typename details::find_type_traits_option< O9,
                                            typename details::find_type_traits_option< O10,
                                                typename details::find_type_traits_option< O11,
                                                    typename details::find_type_traits_option< O12,
                                                        typename details::find_type_traits_option< O13,
                                                            typename details::find_type_traits_option< O14,
                                                                typename details::find_type_traits_option< O15,
                                                                    typename details::find_type_traits_option< O16, DefaultOptions>::type
                                                                >::type
                                                            >::type
                                                        >::type
                                                    >::type
                                                >::type
                                            >::type
                                        >::type
                                    >::type
                                >::type
                            >::type
                        >::type
                    >::type
                >::type
            >::type
        >::type
        {};
    };


    // *****************************************************************
    // find_option metafunction
    // *****************************************************************
    namespace details {

        template <typename A, typename B>
        struct select_option
        {
            typedef void type;
        };

        template <template <typename> class What, typename Option, typename Default>
        struct select_option< What<Option>, What<Default> >
        {
            typedef What<Option>   type;
        };

        // Specializations for integral type of option
#define _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION( _type ) template <template <_type> class What, _type Option, _type Default> \
        struct select_option< What<Option>, What<Default> > { typedef What<Option>   type    ; };

// For user-defined enum types
#define CDS_DECLARE_FIND_OPTION_INTEGRAL_SPECIALIZATION( _type ) namespace cds { namespace opt { namespace details { _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(_type ) }}}

        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(bool)
        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(char)
        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(unsigned char)
        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(signed char)
        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(short int)
        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(unsigned short int)
        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(int)
        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(unsigned int)
        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(long)
        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(unsigned long)
        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(long long)
        _CDS_FIND_OPTION_INTEGRAL_SPECIALIZATION(unsigned long long)


        template <typename What, typename Opt, typename Result>
        struct find_option
        {
            typedef typename cds::details::void_selector< typename select_option< Opt, What >::type, Result >::type type;
        };

        template <typename What, typename Opt>
        struct find_option< What, Opt, void>
        {
            typedef typename select_option< Opt, What >::type  type;
        };

        template <typename What, typename Result>
        struct find_option_decision
        {
            typedef Result type;
        };

        template <typename What>
        struct find_option_decision<What, void>
        {
            typedef What type;
        };


    } // namespace details

    template <
        typename What
        ,typename O1 = none
        ,typename O2 = none
        ,typename O3 = none
        ,typename O4 = none
        ,typename O5 = none
        ,typename O6 = none
        ,typename O7 = none
        ,typename O8 = none
        ,typename O9 = none
        ,typename O10 = none
        ,typename O11 = none
        ,typename O12 = none
        ,typename O13 = none
        ,typename O14 = none
        ,typename O15 = none
        ,typename O16 = none
    >
    struct find_option {
        typedef typename details::find_option_decision< What,
             typename details::find_option< What, O1,
                typename details::find_option< What, O2,
                    typename details::find_option< What, O3,
                        typename details::find_option< What, O4,
                            typename details::find_option< What, O5,
                                typename details::find_option< What, O6,
                                    typename details::find_option< What, O7,
                                        typename details::find_option< What, O8,
                                            typename details::find_option< What, O9,
                                                typename details::find_option< What, O10,
                                                    typename details::find_option< What, O11,
                                                        typename details::find_option< What, O12,
                                                            typename details::find_option< What, O13,
                                                                typename details::find_option< What, O14,
                                                                    typename details::find_option< What, O15,
                                                                        typename details::find_option< What, O16, void >::type
                                                                    >::type
                                                                >::type
                                                            >::type
                                                        >::type
                                                    >::type
                                                >::type
                                            >::type
                                        >::type
                                    >::type
                                >::type
                            >::type
                        >::type
                    >::type
                >::type
            >::type
        >::type type;
    };


    // *****************************************************************
    // select metafunction
    // *****************************************************************

    namespace details {

        template <typename What, typename Tag, typename Value, typename Others>
        struct select
        {
            typedef typename Others   type;
        };

        template <typename What, typename Value, typename Others>
        struct select< What, What, Value, Others>
        {
            typedef Value   type;
        };

        template <typename What, typename Tag, typename Value>
        struct select< What, Tag, Value, void>
        {
            typedef What    type;
        };

        template <typename What, typename Value>
        struct select< What, What, Value, void>
        {
            typedef Value    type;
        };
    }   // namespace details

    template <typename What,
        typename Tag1, typename Value1,
        typename Tag2 = none, typename Value2 = none,
        typename Tag3 = none, typename Value3 = none,
        typename Tag4 = none, typename Value4 = none,
        typename Tag5 = none, typename Value5 = none,
        typename Tag6 = none, typename Value6 = none,
        typename Tag7 = none, typename Value7 = none,
        typename Tag8 = none, typename Value8 = none
    >
    struct select {
        typedef typename details::select< What, Tag1, Value1,
            typename details::select< What, Tag2, Value2,
                typename details::select< What, Tag3, Value3,
                    typename details::select< What, Tag4, Value4,
                        typename details::select< What, Tag5, Value5,
                            typename details::select< What, Tag6, Value6,
                                typename details::select< What, Tag7, Value7,
                                    typename details::select< What, Tag8, Value8, void
                                    >::type
                                >::type
                            >::type
                        >::type
                    >::type
                >::type
            >::type
        >::type type;
    };


}}  // namespace cds::opt
//@endcond

#endif // #ifndef __CDS_OPT_MAKE_OPTIONS_STD_H
