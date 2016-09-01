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
	template<int DefaultSize = 10000>
	struct HeavyValue {
		static std::vector<int> pop_buff;
		int value;
		size_t buffer_size;

		size_t nNo;
		size_t nWriterNo;

		explicit HeavyValue(int new_value = 0, size_t new_bufer_size = DefaultSize)
		: value(new_value),
		  buffer_size(new_bufer_size),
		  nNo(0),
		  nWriterNo(0)

		{
			if( buffer_size != pop_buff.size() ){
				pop_buff.resize(buffer_size);
			}
		};
		HeavyValue(const HeavyValue &other)
			: value(other.value),
			  buffer_size(other.buffer_size)
		{
			working(other);
		}
		void operator=(const int& new_value)
		{
			value = new_value;
		}
		bool operator==(const int new_value) const
		{
			return value == new_value;
		}
		void working(const HeavyValue &other) {
			for (size_t i = 0; i < buffer_size; ++i)
				pop_buff[i] =  static_cast<int>(std::sqrt(other.pop_buff[i]));
		}
	};

	template<int DefaultSize>
	std::vector<int> HeavyValue< DefaultSize >::pop_buff = {};
}
#endif /* SOURCE_DIRECTORY__TEST_INCLUDE_CDS_TEST_FC_HEVY_VALUE_H_ */
