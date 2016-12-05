/*
 * fc_hevy_value.h
 *
 *  Created on: 31 авг. 2016 г.
 *      Author: marsel
 */

#ifndef SOURCE_DIRECTORY__TEST_INCLUDE_CDS_TEST_FC_HEAVY_VALUE_H_
#define SOURCE_DIRECTORY__TEST_INCLUDE_CDS_TEST_FC_HEAVY_VALUE_H_

#include <math.h>
#include <vector>

namespace fc_test {

    // SFINAE test
    template <typename T>
    class has_set_array_size {
        typedef char small;
        class big{char dummy[2];};

        template <typename C, void (C::*) (size_t)> class SFINAE {};

        template <typename C> static small test( SFINAE<C, &C::set_array> * ) ;
        template <typename C> static big   test(...);

    public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(char) ;
    };

    template<int DefaultSize = 10>
    struct HeavyValue {

        int value;

        size_t nNo;
        size_t nWriterNo;

        static std::vector<int> pop_buff;
        static size_t buffer_size;

        explicit HeavyValue(int new_value = 0)
        : value(new_value),
          nNo(0),
          nWriterNo(0)
        {
        };
        HeavyValue(const HeavyValue &other)
            : value(other.value),
              nNo(other.nNo),
              nWriterNo(other.nWriterNo)
        {
            for(size_t i = 0; i < buffer_size; ++i)
                pop_buff[i] =  static_cast<int>(std::sqrt(other.pop_buff[i]*rand()));
        }
        void set_array(size_t new_size) {
            set_array_size(new_size);
        }
        static void set_array_size(size_t new_size){
            if (buffer_size == new_size) return;
            buffer_size = new_size;
            pop_buff.resize(buffer_size, rand());
        }
    };
    template<int DefaultSize>
    std::vector<int> HeavyValue< DefaultSize >::pop_buff(DefaultSize, rand());
    template<int DefaultSize>
    std::vector<int>::size_type HeavyValue< DefaultSize >::buffer_size = DefaultSize;
}
#endif /* SOURCE_DIRECTORY__TEST_INCLUDE_CDS_TEST_FC_HEVY_VALUE_H_ */
