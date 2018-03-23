// Copyright (c) 2016 Marsel Galimullin
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDS_TEST_FC_HEAVY_VALUE_H_
#define CDS_TEST_FC_HEAVY_VALUE_H_

#include <cmath>
#include <vector>

namespace fc_test {

    // SFINAE test
    template <typename T>
    class has_set_array_size {
        typedef char select_small;
        class select_big {
            char dummy[2];
        };

        template <typename C, void (C::*) (size_t)> class selector
        {};

        template <typename C> static select_small test( selector<C, &C::set_array>* );
        template <typename C> static select_big   test(...);

    public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(char);
    };

    template<int DefaultSize = 10>
    struct heavy_value {

        int value;

        size_t nNo;
        size_t nWriterNo;

        static std::vector<int> pop_buff;
        static size_t buffer_size;

        explicit heavy_value(int new_value = 0)
        : value(new_value),
          nNo(0),
          nWriterNo(0)
        {};

        heavy_value( heavy_value const& other)
            : value(other.value)
            , nNo(other.nNo)
            , nWriterNo(other.nWriterNo)
        {
            // This is an imitation of heavy copy ctor
            for(size_t i = 0; i < buffer_size; ++i)
                pop_buff[i] = static_cast<int>( std::sqrt( std::abs( static_cast<double>( pop_buff[i] ) * rand())));
        }

        void set_array(size_t new_size)
        {
            set_array_size(new_size);
        }

        static void set_array_size(size_t new_size)
        {
            if (buffer_size == new_size) return;
            buffer_size = new_size;
            pop_buff.resize(buffer_size, rand());
        }
    };

    template<int DefaultSize>
    std::vector<int> heavy_value< DefaultSize >::pop_buff(DefaultSize, rand());
    template<int DefaultSize>
    std::vector<int>::size_type heavy_value< DefaultSize >::buffer_size = DefaultSize;
}
#endif // CDS_TEST_FC_HEAVY_VALUE_H_
