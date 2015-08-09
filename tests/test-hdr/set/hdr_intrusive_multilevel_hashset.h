//$$CDS-header$$

#ifndef CDSTEST_HDR_INTRUSIVE_MULTILEVEL_HASHSET_H
#define CDSTEST_HDR_INTRUSIVE_MULTILEVEL_HASHSET_H

#include "cppunit/cppunit_proxy.h"

// forward declaration
namespace cds { 
    namespace intrusive {}
    namespace opt {}
}

namespace set {
    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    class IntrusiveMultiLevelHashSetHdrTest: public CppUnitMini::TestCase
    {
        template <typename Hash>
        struct Item 
        {
            unsigned int nDisposeCount  ;   // count of disposer calling
            Hash hash;
            unsigned int nInsertCall;
            unsigned int nFindCall;
            unsigned int nEraseCall;

            Item()
                : nDisposeCount(0)
                , nInsertCall(0)
                , nFindCall(0)
                , nEraseCall(0)
            {}
        };

        template <typename Hash>
        struct get_hash
        {
            Hash const& operator()( Item<Hash> const& i ) const
            {
                return i.hash;
            }
        };

        struct item_disposer {
            template <typename Hash>
            void operator()( Item<Hash> * p )
            {
                ++p->nDisposeCount;
            }
        };

        template <typename Set>
        void test_hp()
        {
            typedef typename Set::hash_type hash_type;
            typedef typename Set::value_type value_type;

            std::hash<hash_type> hasher;

            size_t const arrCapacity = 1000;
            std::vector< value_type > arrValue;
            arrValue.reserve( arrCapacity );
            for ( size_t i = 0; i < arrCapacity; ++i ) {
                arrValue.emplace_back( value_type() );
                arrValue.back().hash = hasher( i );
            }
            CPPUNIT_ASSERT( arrValue.size() == arrCapacity );

            Set s( 4, 2 );
            CPPUNIT_ASSERT(s.head_size() == 16 );
            CPPUNIT_ASSERT(s.array_node_size() == 4 );

            // insert() test
            CPPUNIT_ASSERT(s.size() == 0 );
            CPPUNIT_ASSERT(s.empty() );
            for ( auto& el : arrValue ) {
                CPPUNIT_ASSERT( s.insert( el ));
                CPPUNIT_ASSERT(s.contains( el.hash ));
            }
            CPPUNIT_ASSERT(s.size() == arrCapacity );
            for ( auto& el : arrValue ) {
                CPPUNIT_ASSERT(s.contains( el.hash ));
                CPPUNIT_ASSERT( !s.insert( el ) );
            }
            CPPUNIT_ASSERT(s.size() == arrCapacity );
            CPPUNIT_ASSERT( !s.empty() );

            // update() exists test
            for ( auto& el : arrValue ) {
                bool bOp, bInsert;
                std::tie(bOp, bInsert) = s.update( el, false );
                CPPUNIT_ASSERT( bOp );
                CPPUNIT_ASSERT( !bInsert );
                CPPUNIT_ASSERT( el.nFindCall == 0 );
                CPPUNIT_ASSERT(s.find(el.hash, [](value_type& v) { v.nFindCall++; } ));
                CPPUNIT_ASSERT( el.nFindCall == 1 );
            }

            // unlink test
            CPPUNIT_ASSERT(s.size() == arrCapacity );
            for ( auto const& el : arrValue ) {
                CPPUNIT_ASSERT(s.unlink( el ));
                CPPUNIT_ASSERT(!s.contains( el.hash ));
            }
            CPPUNIT_ASSERT(s.size() == 0 );
            Set::gc::force_dispose();
            for ( auto const& el : arrValue ) {
                CPPUNIT_ASSERT( el.nDisposeCount == 1 );
            }

            // new hash values
            for ( auto& el : arrValue )
                el.hash = hasher( el.hash );

            // insert( func )
            CPPUNIT_ASSERT(s.size() == 0 );
            for ( auto& el : arrValue ) {
                CPPUNIT_ASSERT( s.insert( el, []( value_type& v ) { ++v.nInsertCall; } ));
                CPPUNIT_ASSERT(s.contains( el.hash ));
                CPPUNIT_ASSERT( el.nInsertCall == 1 );
            }
            CPPUNIT_ASSERT(s.size() == arrCapacity );
            for ( auto& el : arrValue ) {
                CPPUNIT_ASSERT(s.contains( el.hash ));
                CPPUNIT_ASSERT( !s.insert( el ) );
            }
            CPPUNIT_ASSERT(s.size() == arrCapacity );
            CPPUNIT_ASSERT( !s.empty() );

            for ( auto& el : arrValue )
                el.nDisposeCount = 0;

            s.clear();
            CPPUNIT_ASSERT(s.size() == 0 );
            Set::gc::force_dispose();
            for ( auto const& el : arrValue ) {
                CPPUNIT_ASSERT( el.nDisposeCount == 1 );
            }

            // new hash values
            for ( auto& el : arrValue )
                el.hash = hasher( el.hash );

            // update test
            for ( auto& el : arrValue ) {
                bool bOp, bInsert;
                std::tie(bOp, bInsert) = s.update( el, false );
                CPPUNIT_ASSERT( !bOp );
                CPPUNIT_ASSERT( !bInsert );
                CPPUNIT_ASSERT( !s.contains( el.hash ));

                std::tie(bOp, bInsert) = s.update( el, true );
                CPPUNIT_ASSERT( bOp );
                CPPUNIT_ASSERT( bInsert );
                CPPUNIT_ASSERT( s.contains( el.hash ));
            }
            CPPUNIT_ASSERT(s.size() == arrCapacity );

            // erase test
            for ( auto& el : arrValue ) {
                el.nDisposeCount = 0;
                CPPUNIT_ASSERT( s.contains( el.hash ));
                CPPUNIT_ASSERT(s.erase( el.hash ));
                CPPUNIT_ASSERT( !s.contains( el.hash ));
                CPPUNIT_ASSERT( !s.erase( el.hash ));
            }
            CPPUNIT_ASSERT(s.size() == 0 );
            Set::gc::force_dispose();
            for ( auto& el : arrValue ) {
                CPPUNIT_ASSERT( el.nDisposeCount == 1 );
                CPPUNIT_ASSERT(s.insert( el ));
            }

            // erase with functor, get() test
            for ( auto& el : arrValue ) {
                el.nDisposeCount = 0;
                CPPUNIT_ASSERT( s.contains( el.hash ) );
                {
                    typename Set::guarded_ptr gp{ s.get( el.hash ) };
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( gp->nEraseCall == 0);
                    CPPUNIT_ASSERT(s.erase( gp->hash, []( value_type& i ) { ++i.nEraseCall; } ));
                    CPPUNIT_ASSERT( gp->nEraseCall == 1);
                    Set::gc::force_dispose();
                    CPPUNIT_ASSERT( gp->nDisposeCount == 0 );
                }
                CPPUNIT_ASSERT( !s.contains( el.hash ));
                CPPUNIT_ASSERT( !s.erase( el.hash ));
                CPPUNIT_ASSERT( el.nEraseCall == 1 );
                Set::gc::force_dispose();
                CPPUNIT_ASSERT( el.nDisposeCount == 1 );
            }
            CPPUNIT_ASSERT(s.size() == 0 );

            // new hash values
            for ( auto& el : arrValue ) {
                el.hash = hasher( el.hash );
                el.nDisposeCount = 0;
                bool bOp, bInsert;
                std::tie(bOp, bInsert) = s.update( el );
                CPPUNIT_ASSERT( bOp );
                CPPUNIT_ASSERT( bInsert );
            }
            CPPUNIT_ASSERT(s.size() == arrCapacity );

            // extract test
            for ( auto& el : arrValue ) {
                CPPUNIT_ASSERT( s.contains( el.hash ) );
                typename Set::guarded_ptr gp = s.extract( el.hash );
                CPPUNIT_ASSERT( gp );
                Set::gc::force_dispose();
                CPPUNIT_ASSERT( el.nDisposeCount == 0 );
                CPPUNIT_ASSERT( gp->nDisposeCount == 0 );
                gp = s.get( el.hash );
                CPPUNIT_ASSERT( !gp );
                Set::gc::force_dispose();
                CPPUNIT_ASSERT( el.nDisposeCount == 1 );
                CPPUNIT_ASSERT( !s.contains( el.hash ) );
            }
            CPPUNIT_ASSERT(s.size() == 0 );
            CPPUNIT_ASSERT(s.empty() );

            CPPUNIT_MSG( s.statistics() );
        }

        void hp_stdhash();
        void hp_stdhash_stat();

        CPPUNIT_TEST_SUITE(IntrusiveMultiLevelHashSetHdrTest)
            CPPUNIT_TEST(hp_stdhash)
            CPPUNIT_TEST(hp_stdhash_stat)
        CPPUNIT_TEST_SUITE_END()
    };
} // namespace set

#endif // #ifndef CDSTEST_HDR_INTRUSIVE_MULTILEVEL_HASHSET_H
