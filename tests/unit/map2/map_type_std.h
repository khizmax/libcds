/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#ifndef CDSUNIT_MAP_TYPE_STD_H
#define CDSUNIT_MAP_TYPE_STD_H

#include "map2/map_type.h"

#include "map2/std_map.h"
#include "map2/std_hash_map.h"

#include "lock/nolock.h"

namespace map2 {

    struct tag_StdMap;

    template <typename Key, typename Value>
    struct map_type< tag_StdMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::compare    compare;
        typedef typename base_class::less       less;

        typedef StdMap< Key, Value, cds::sync::spin >   StdMap_Spin;
        typedef StdMap< Key, Value, std::mutex >        StdMap_Mutex;
        typedef StdMap< Key, Value, lock::NoLock>       StdMap_NoLock;

        typedef StdHashMap< Key, Value, cds::sync::spin > StdHashMap_Spin;
        typedef StdHashMap< Key, Value, std::mutex >      StdHashMap_Mutex;
        typedef StdHashMap< Key, Value, lock::NoLock >    StdHashMap_NoLock;
    };
}   // namespace map2

#endif // ifndef CDSUNIT_MAP_TYPE_STD_H
