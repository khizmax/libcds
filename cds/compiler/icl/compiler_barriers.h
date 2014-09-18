//$$CDS-header$$

#ifndef __CDS_COMPILER_ICL_COMPILER_BARRIERS_H
#define __CDS_COMPILER_ICL_COMPILER_BARRIERS_H

#if _MSC_VER > 0
#   if _MSC_VER < 1700
        // VC++ up to vc10
#       include <intrin.h>

#       pragma intrinsic(_ReadWriteBarrier)
#       pragma intrinsic(_ReadBarrier)
#       pragma intrinsic(_WriteBarrier)

#       define CDS_COMPILER_RW_BARRIER  _ReadWriteBarrier()
#       define CDS_COMPILER_R_BARRIER   _ReadBarrier()
#       define CDS_COMPILER_W_BARRIER   _WriteBarrier()

#   else
        // MS VC11+
#       include <atomic>

#       define CDS_COMPILER_RW_BARRIER  std::atomic_thread_fence( std::memory_order_acq_rel )
#       define CDS_COMPILER_R_BARRIER   CDS_COMPILER_RW_BARRIER
#       define CDS_COMPILER_W_BARRIER   CDS_COMPILER_RW_BARRIER

#   endif
#endif

#endif  // #ifndef __CDS_COMPILER_ICL_COMPILER_BARRIERS_H
