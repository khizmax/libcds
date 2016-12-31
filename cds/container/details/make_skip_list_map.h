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

#ifndef CDSLIB_CONTAINER_DETAILS_MAKE_SKIP_LIST_MAP_H
#define CDSLIB_CONTAINER_DETAILS_MAKE_SKIP_LIST_MAP_H

#include <cds/container/details/skip_list_base.h>
#include <cds/details/binary_functor_wrapper.h>

//@cond
namespace cds { namespace container { namespace details {

    template <typename GC, typename K, typename T, typename Traits>
    struct make_skip_list_map
    {
        typedef GC      gc;
        typedef K       key_type;
        typedef T       mapped_type;
        typedef std::pair< key_type const, mapped_type> value_type;
        typedef Traits  traits;

        typedef cds::intrusive::skip_list::node< gc >   intrusive_node_type;
        struct node_type: public intrusive_node_type
        {
            typedef intrusive_node_type                     base_class;
            typedef typename base_class::atomic_marked_ptr  atomic_marked_ptr;
            typedef value_type                              stored_value_type;

            value_type m_Value;
            //atomic_marked_ptr m_arrTower[] ;  // allocated together with node_type in single memory block

            template <typename Q>
            node_type( unsigned int nHeight, atomic_marked_ptr * pTower, Q&& key )
                : m_Value( std::make_pair( std::forward<Q>( key ), mapped_type()))
            {
                init_tower( nHeight, pTower );
            }

            template <typename Q, typename... Args>
            node_type( unsigned int nHeight, atomic_marked_ptr * pTower, Q&& key, Args&&... args )
                : m_Value( std::forward<Q>(key), mapped_type( std::forward<Args>(args)... ))
            {
                init_tower( nHeight, pTower );
            }

            node_type() = delete;

        private:
            void init_tower( unsigned int nHeight, atomic_marked_ptr * pTower )
            {
                if ( nHeight > 1 ) {
                    // TSan: make_tower() issues atomic_thread_fence( release )
                    CDS_TSAN_ANNOTATE_IGNORE_WRITES_BEGIN;
                    new (pTower) atomic_marked_ptr[ nHeight - 1 ];
                    base_class::make_tower( nHeight, pTower );
                    CDS_TSAN_ANNOTATE_IGNORE_WRITES_END;
                }
            }
        };

        class node_allocator : public skip_list::details::node_allocator< node_type, traits>
        {
            typedef skip_list::details::node_allocator< node_type, traits> base_class;
        public:
            template <typename Q>
            node_type * New( unsigned int nHeight, Q const& key )
            {
                return base_class::New( nHeight, key_type( key ));
            }
            template <typename Q, typename U>
            node_type * New( unsigned int nHeight, Q const& key, U const& val )
            {
                unsigned char * pMem = base_class::alloc_space( nHeight );
                return new( pMem )
                    node_type( nHeight,
                        nHeight > 1 ? reinterpret_cast<typename base_class::node_tower_item *>( pMem + base_class::c_nNodeSize ) : nullptr,
                        key_type( key ), mapped_type( val )
                    );
            }
            template <typename... Args>
            node_type * New( unsigned int nHeight, Args&&... args )
            {
                unsigned char * pMem = base_class::alloc_space( nHeight );
                return new( pMem )
                    node_type( nHeight,
                        nHeight > 1 ? reinterpret_cast<typename base_class::node_tower_item *>( pMem + base_class::c_nNodeSize ) : nullptr,
                        std::forward<Args>(args)...
                    );
            }
        };

        struct node_deallocator {
            void operator ()( node_type * pNode )
            {
                node_allocator().Delete( pNode );
            }
        };

        typedef skip_list::details::dummy_node_builder<intrusive_node_type> dummy_node_builder;

        struct key_accessor
        {
            key_type const & operator()( node_type const& node ) const
            {
                return node.m_Value.first;
            }
        };
        typedef typename opt::details::make_comparator< key_type, traits >::type key_comparator;

        class intrusive_type_traits: public cds::intrusive::skip_list::make_traits<
            cds::opt::type_traits< traits >
            ,cds::intrusive::opt::hook< intrusive::skip_list::base_hook< cds::opt::gc< gc > > >
            ,cds::intrusive::opt::disposer< node_deallocator >
            ,cds::intrusive::skip_list::internal_node_builder< dummy_node_builder >
            ,cds::opt::compare< cds::details::compare_wrapper< node_type, key_comparator, key_accessor > >
        >::type
        {};

        typedef cds::intrusive::SkipListSet< gc, node_type, intrusive_type_traits>   type;
    };

}}} // namespace cds::container::details
//@endcond

#endif // CDSLIB_CONTAINER_DETAILS_MAKE_SKIP_LIST_MAP_H
