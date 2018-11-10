// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_VERSION_H
#define CDSLIB_VERSION_H

// cds library version

#define CDS_VERSION        0x020303

#define CDS_VERSION_MAJOR  ((CDS_VERSION & 0xFF0000)>> 16)
#define CDS_VERSION_MINOR  ((CDS_VERSION & 0x00FF00) >> 8)
#define CDS_VERSION_PATCH  (CDS_VERSION & 0x0000FF)

// CDS_VERSION == CDS_VERSION_MAJOR '.' CDS_VERSION_MINOR '.' CDS_VERSION_PATCH
#define CDS_VERSION_STRING        "2.3.3"

#endif  // #ifndef CDSLIB_VERSION_H

