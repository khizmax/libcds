//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_SKIP_LIST_HRC_H
#define __CDS_INTRUSIVE_SKIP_LIST_HRC_H

#include <cds/intrusive/skip_list_impl.h>
#include <cds/gc/hrc.h>

//@cond
namespace cds { namespace intrusive { namespace skip_list {
    // Specialization for HRC GC
    template <typename Tag>
    class node< cds::gc::HRC, Tag>: public cds::gc::HRC::container_node
    {
    public:
        typedef gc::HRC gc          ;   ///< Garbage collector
        typedef Tag     tag         ;   ///< tag

        typedef cds::details::marked_ptr<node, 1>                       marked_ptr          ;   ///< marked pointer
        typedef typename gc::template atomic_marked_ptr< marked_ptr>    atomic_marked_ptr   ;   ///< atomic marked pointer specific for GC
        typedef atomic_marked_ptr   tower_item_type;

    protected:
        atomic_marked_ptr       m_pNext     ;   ///< Next item in bottom-list (list at level 0)
        unsigned int            m_nHeight   ;   ///< Node height (size of m_arrNext array). For node at level 0 the height is 1.
        atomic_marked_ptr *     m_arrNext   ;   ///< Array of next items for levels 1 .. m_nHeight - 1. For node at level 0 \p m_arrNext is \p NULL

    public:
        bool                    m_bDel;

    public:
        /// Constructs a node of height 1 (a bottom-list node)
        node()
            : m_pNext( null_ptr<node *>())
            , m_nHeight(1)
            , m_arrNext( null_ptr<atomic_marked_ptr *>())
            , m_bDel( false )
        {}

        ~node()
        {
            release_tower();
            m_pNext.store( marked_ptr(), CDS_ATOMIC::memory_order_relaxed );
        }

        /// Constructs a node of height \p nHeight
        void make_tower( unsigned int nHeight, atomic_marked_ptr * nextTower )
        {
            assert( nHeight > 0 );
            assert( ( nHeight == 1 && nextTower == null_ptr<atomic_marked_ptr *>() )  // bottom-list node
                || ( nHeight > 1  && nextTower != null_ptr<atomic_marked_ptr *>() )   // node at level of more than 0
                );

            m_arrNext = nextTower;
            m_nHeight = nHeight;
        }

        atomic_marked_ptr * release_tower()
        {
            unsigned int nHeight = m_nHeight - 1;
            atomic_marked_ptr * pTower = m_arrNext;
            if ( pTower ) {
                m_arrNext = null_ptr<atomic_marked_ptr *>();
                m_nHeight = 1;
                for ( unsigned int i = 0; i < nHeight; ++i )
                    pTower[i].store( marked_ptr(), CDS_ATOMIC::memory_order_release );
            }
            return pTower;
        }

        atomic_marked_ptr * get_tower() const
        {
            return m_arrNext;
        }

        /// Access to element of next pointer array
        atomic_marked_ptr& next( unsigned int nLevel )
        {
            assert( nLevel < height() );
            assert( nLevel == 0 || (nLevel > 0 && m_arrNext != null_ptr<atomic_marked_ptr *>() ));

            return nLevel ? m_arrNext[ nLevel - 1] : m_pNext;
        }

        /// Access to element of next pointer array (const version)
        atomic_marked_ptr const& next( unsigned int nLevel ) const
        {
            assert( nLevel < height() );
            assert( nLevel == 0 || (nLevel > 0 && m_arrNext != null_ptr<atomic_marked_ptr *>()) );

            return nLevel ? m_arrNext[ nLevel - 1] : m_pNext;
        }

        /// Access to element of next pointer array (same as \ref next function)
        atomic_marked_ptr& operator[]( unsigned int nLevel )
        {
            return next( nLevel );
        }

        /// Access to element of next pointer array (same as \ref next function)
        atomic_marked_ptr const& operator[]( unsigned int nLevel ) const
        {
            return next( nLevel );
        }

        /// Height of the node
        unsigned int height() const
        {
            return m_nHeight;
        }

    protected:
        virtual void cleanUp( cds::gc::hrc::ThreadGC * pGC )
        {
            assert( pGC != NULL );
            typename gc::GuardArray<2> aGuards( *pGC );

            unsigned int const nHeight = height();
            for (unsigned int i = 0; i < nHeight; ++i ) {
                while ( true ) {
                    marked_ptr pNextMarked( aGuards.protect( 0, next(i) ));
                    node * pNext = pNextMarked.ptr();
                    if ( pNext && pNext->m_bDeleted.load(CDS_ATOMIC::memory_order_acquire) ) {
                        marked_ptr p = aGuards.protect( 1, pNext->next(i) );
                        next(i).compare_exchange_strong( pNextMarked, p, CDS_ATOMIC::memory_order_acquire, CDS_ATOMIC::memory_order_relaxed );
                        continue;
                    }
                    else {
                        break;
                    }
                }
            }
        }

        virtual void terminate( cds::gc::hrc::ThreadGC * pGC, bool bConcurrent )
        {
            unsigned int const nHeight = height();
            if ( bConcurrent ) {
                for (unsigned int i = 0; i < nHeight; ++i ) {
                    marked_ptr pNext = next(i).load(CDS_ATOMIC::memory_order_relaxed);
                    while ( !next(i).compare_exchange_weak( pNext, marked_ptr(), CDS_ATOMIC::memory_order_release, CDS_ATOMIC::memory_order_relaxed ) );
                }
            }
            else {
                for (unsigned int i = 0; i < nHeight; ++i )
                    next(i).store( marked_ptr(), CDS_ATOMIC::memory_order_relaxed );
            }
        }
    };

    namespace details {

        template <typename Tag>
        class head_node< node< cds::gc::HRC, Tag > >
        {
            typedef node< cds::gc::HRC, Tag > node_type;

            struct head_tower: public node_type
            {
                typename node_type::atomic_marked_ptr   m_Tower[skip_list::c_nHeightLimit];
            };

            head_tower * m_pHead;

            struct head_disposer {
                void operator()( head_tower * p )
                {
                    delete p;
                }
            };
        public:
            head_node( unsigned int nHeight )
                : m_pHead( new head_tower() )
            {
                for ( size_t i = 0; i < sizeof(m_pHead->m_Tower) / sizeof(m_pHead->m_Tower[0]); ++i )
                    m_pHead->m_Tower[i].store( typename node_type::marked_ptr(), CDS_ATOMIC::memory_order_relaxed );

                m_pHead->make_tower( nHeight, m_pHead->m_Tower );
            }

            ~head_node()
            {
                cds::gc::HRC::template retire<head_disposer>( m_pHead );
            }

            node_type * head()
            {
                return static_cast<node_type *>( m_pHead );
            }
            node_type const * head() const
            {
                return static_cast<node_type const *>( m_pHead );
            }

        };
    } // namespace details

}}} // namespace cds::intrusive::skip_list
//@endcond

#endif
