// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_USER_SETUP_THREADING_MODEL_H
#define CDSLIB_USER_SETUP_THREADING_MODEL_H

/**
    CDS threading model

    CDS_THREADING_AUTODETECT - auto-detect appropriate threading model (default)

    CDS_THREADING_MSVC - use MS Visual C++ declspec( thread ) declaration to mantain thread-specific data

    CDS_THREADING_WIN_TLS - use Windows TLS API to mantain thread-specific data

    CDS_THREADING_GCC - use GCC __thread keyword to mantain thread-specific data

    CDS_THREADING_PTHREAD - use cds::Threading::Manager implementation based on pthread thread-specific
    data functions pthread_getspecific/pthread_setspecific

    CDS_THREADING_USER_DEFINED - use user-defined threading model
*/
#define CDS_THREADING_AUTODETECT

#endif    // #ifndef CDSLIB_USER_SETUP_THREADING_MODEL_H
