// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSSTRESS_PQUEUE_ITEM_H
#define CDSSTRESS_PQUEUE_ITEM_H

namespace pqueue {

    struct simple_value {
        typedef size_t  key_type;

        key_type key;

        struct key_extractor {
            void operator()( key_type& k, simple_value const& s ) const
            {
                k = s.key;
            }
        };

        simple_value()
            : key(0)
        {}

        simple_value( key_type n )
            : key(n)
        {}

        friend bool operator <( simple_value const& lhs, simple_value const& rhs )
        {
            return lhs.key < rhs.key;
        }
        friend bool operator <( simple_value const& lhs, size_t rhs )
        {
            return lhs.key < rhs;
        }
        friend bool operator <( size_t lhs, simple_value const& rhs )
        {
            return lhs < rhs.key;
        }

        friend bool operator >( simple_value const& lhs, simple_value const& rhs )
        {
            return lhs.key > rhs.key;
        }
        friend bool operator >( simple_value const& lhs, size_t rhs )
        {
            return lhs.key > rhs;
        }
        friend bool operator >( size_t lhs, simple_value const& rhs )
        {
            return lhs > rhs.key;
        }
    };
} // namespace pqueue

#endif // #ifndef CDSSTRESS_PQUEUE_ITEM_H
