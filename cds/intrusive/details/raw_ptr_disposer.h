// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_DETAILS_RAW_PTR_DISPOSER_H
#define CDSLIB_INTRUSIVE_DETAILS_RAW_PTR_DISPOSER_H

#include <cds/details/defs.h>

//@cond
namespace cds { namespace intrusive { namespace details {

    template <typename RCU, typename NodeType, typename Disposer>
    struct raw_ptr_disposer
    {
        typedef RCU gc;
        typedef NodeType node_type;
        typedef Disposer disposer;

        node_type *     pReclaimedChain;

        raw_ptr_disposer()
            : pReclaimedChain( nullptr )
        {}

        template <typename Position>
        explicit raw_ptr_disposer( Position& pos )
            : pReclaimedChain( pos.pDelChain )
        {
            pos.pDelChain = nullptr;
        }

        raw_ptr_disposer( raw_ptr_disposer&& d )
            : pReclaimedChain( d.pReclaimedChain )
        {
            d.pReclaimedChain = nullptr;
        }

        raw_ptr_disposer( raw_ptr_disposer const& ) = delete;

        ~raw_ptr_disposer()
        {
            apply();
        }

        raw_ptr_disposer& combine(raw_ptr_disposer&& d)
        {
            if ( pReclaimedChain == nullptr )
                pReclaimedChain = d.pReclaimedChain;
            else if ( d.pReclaimedChain ) {
                // union reclaimed chains
                node_type * pEnd = d.pReclaimedChain;
                for ( ; pEnd->m_pDelChain; pEnd = pEnd->m_pDelChain );
                pEnd->m_pDelChain = pReclaimedChain;
                pReclaimedChain = d.pReclaimedChain;
            }
            d.pReclaimedChain = nullptr;
            return *this;
        }

        raw_ptr_disposer& operator=(raw_ptr_disposer const& d) = delete;
        raw_ptr_disposer& operator=( raw_ptr_disposer&& d ) = delete;

        void apply()
        {
            if ( pReclaimedChain ) {
                assert( !gc::is_locked());
                disposer()( pReclaimedChain );
                pReclaimedChain = nullptr;
            }
        }
    };

}}} // namespace cds::intrusive::details
//@endcond

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_RAW_PTR_DISPOSER_H
