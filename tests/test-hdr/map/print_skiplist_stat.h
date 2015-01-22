//$$CDS-header$$

#ifndef CDSTEST_HDR_MAP_PRINT_SKIPLIST_STAT_H
#define CDSTEST_HDR_MAP_PRINT_SKIPLIST_STAT_H

#include "unit/print_skip_list_stat.h"

namespace misc {

    template <typename Stat>
    struct print_skiplist_stat;

    template <>
    struct print_skiplist_stat< cds::intrusive::skip_list::stat<> >
    {
        template <class Set>
        std::string operator()( Set const& s, const char * pszHdr )
        {
            std::stringstream st;
            if ( pszHdr ) {
                st << "\t\t" << pszHdr << "\n"
                    << s.statistics()
;
            }
            else {
                st << s.statistics();
            }
            return st.str();
        }
    };

    template<>
    struct print_skiplist_stat< cds::intrusive::skip_list::empty_stat >
    {
        template <class Set>
        std::string operator()( Set const& /*s*/, const char * /*pszHdr*/ )
        {
            return std::string();
        }
    };

}   // namespace misc

#endif // #ifndef CDSTEST_HDR_MAP_PRINT_SKIPLIST_STAT_H
