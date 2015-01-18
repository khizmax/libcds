/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef __CDS_TESTHDR_MAP_PRINT_SKIPLIST_STAT_H
#define __CDS_TESTHDR_MAP_PRINT_SKIPLIST_STAT_H

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

#endif // #ifndef __CDS_TESTHDR_MAP_PRINT_SKIPLIST_STAT_H
