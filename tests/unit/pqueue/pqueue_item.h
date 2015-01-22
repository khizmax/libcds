//$$CDS-header$$

#ifndef CDSUNIT_PQUEUE_ITEM_H
#define CDSUNIT_PQUEUE_ITEM_H

namespace pqueue {
    struct SimpleValue {
        size_t      key;

        typedef size_t  key_type;

        struct key_extractor {
            void operator()( key_type& k, SimpleValue const& s ) const
            {
                k = s.key;
            }
        };

        SimpleValue(): key(0) {}
        SimpleValue( size_t n ): key(n) {}
    };
}

namespace std {
    template <>
    struct less<pqueue::SimpleValue>
    {
        bool operator()( pqueue::SimpleValue const& k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1.key < k2.key;
        }

        bool operator()( pqueue::SimpleValue const& k1, size_t k2 ) const
        {
            return k1.key < k2;
        }

        bool operator()( size_t k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1 < k2.key;
        }

        bool operator()( size_t k1, size_t k2 ) const
        {
            return k1 < k2;
        }
    };

    template <>
    struct greater<pqueue::SimpleValue>
    {
        bool operator()( pqueue::SimpleValue const& k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1.key > k2.key;
        }

        bool operator()( pqueue::SimpleValue const& k1, size_t k2 ) const
        {
            return k1.key > k2;
        }

        bool operator()( size_t k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1 > k2.key;
        }

        bool operator()( size_t k1, size_t k2 ) const
        {
            return k1 > k2;
        }
    };

} // namespace std

#endif // #ifndef CDSUNIT_PQUEUE_ITEM_H
