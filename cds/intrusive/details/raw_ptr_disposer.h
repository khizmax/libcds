/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
