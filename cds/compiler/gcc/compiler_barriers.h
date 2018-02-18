// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_GCC_COMPILER_BARRIERS_H
#define CDSLIB_COMPILER_GCC_COMPILER_BARRIERS_H

#define CDS_COMPILER_RW_BARRIER  __asm__ __volatile__ ( "" ::: "memory" )
#define CDS_COMPILER_R_BARRIER   CDS_COMPILER_RW_BARRIER
#define CDS_COMPILER_W_BARRIER   CDS_COMPILER_RW_BARRIER

#endif  // #ifndef CDSLIB_COMPILER_GCC_COMPILER_BARRIERS_H
