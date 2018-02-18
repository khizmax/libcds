// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSTEST_BYTESWAP_H
#define CDSTEST_BYTESWAP_H

#ifdef CDSTEST_HAVE_BYTESWAP_H
#   include <byteswap.h>
#else

#define bswap_16(value) ((((value) & 0xff) << 8) | ((value) >> 8))

#define bswap_32(value) (((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | (uint32_t)bswap_16((uint16_t)((value) >> 16)))

#define bswap_64(value) (((uint64_t)bswap_32((uint32_t)((value) & 0xffffffff)) << 32) | (uint64_t)bswap_32((uint32_t)((value) >> 32)))

#endif
#endif // CDSTEST_BYTESWAP_H
