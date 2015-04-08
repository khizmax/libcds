//$$CDS-header$$

#ifndef CDSTEST_HDR_INTRUSIVE_CUCKOO_SET_COMMON_H
#define CDSTEST_HDR_INTRUSIVE_CUCKOO_SET_COMMON_H

namespace set {
    namespace {

        typedef IntrusiveCuckooSetHdrTest::hash_int    hash1;
        struct hash2: private hash1
        {
            typedef hash1 base_class;

            size_t operator()( int i ) const
            {
                size_t h = ~( base_class::operator()(i));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            template <typename Item>
            size_t operator()( const Item& i ) const
            {
                size_t h = ~( base_class::operator()(i));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            size_t operator()( IntrusiveCuckooSetHdrTest::find_key const& i) const
            {
                size_t h = ~( base_class::operator()(i));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
        };

        template <typename T>
        struct equal_to
        {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1.key() == v2.key();
            }

            template <typename Q>
            bool operator ()(const T& v1, const Q& v2 ) const
            {
                return v1.key() == v2;
            }

            template <typename Q>
            bool operator ()(const Q& v1, const T& v2 ) const
            {
                return v1 == v2.key();
            }
        };

    }
} // namespace set

#endif // #ifndef CDSTEST_HDR_INTRUSIVE_CUCKOO_SET_COMMON_H
