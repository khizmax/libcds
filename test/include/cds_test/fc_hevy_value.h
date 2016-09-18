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

namespace{
    template<int DefaultSize = 10>
    struct HeavyValue {

        int value;

        size_t nNo;
        size_t nWriterNo;

        static std::vector<int> pop_buff;
        static std::vector<int>::size_type buffer_size;

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
            for(decltype(buffer_size) i = 0; i < buffer_size; ++i)
                pop_buff[i] =  static_cast<int>(std::sqrt(other.pop_buff[i]*rand()));
        }
        static void setArraySize(decltype(buffer_size) new_size){
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
