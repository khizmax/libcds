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
        };

        template <typename Hash>
        struct get_hash
        {
            Hash const& operator()( Item<Hash> const& i ) const
            {
                return i.hash;
            }
        };

        template <typename Set>
        void test_hp()
        {
            Set s;
        }

        void hp_stdhash();

        CPPUNIT_TEST_SUITE(IntrusiveMultiLevelHashSetHdrTest)
            CPPUNIT_TEST(hp_stdhash)
        CPPUNIT_TEST_SUITE_END()
    };
} // namespace set

#endif // #ifndef CDSTEST_HDR_INTRUSIVE_MULTILEVEL_HASHSET_H
