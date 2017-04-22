/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#ifndef CDSLIB_COMPILER_FEATURE_TSAN_H
#define CDSLIB_COMPILER_FEATURE_TSAN_H

// Thread Sanitizer annotations.
// From http://llvm.org/viewvc/llvm-project/compiler-rt/trunk/test/tsan/annotate_happens_before.cc?view=markup

//@cond

#ifdef CDS_THREAD_SANITIZER_ENABLED
#   define CDS_TSAN_ANNOTATE_HAPPENS_BEFORE(addr)   AnnotateHappensBefore(__FILE__, __LINE__, reinterpret_cast<void*>(addr))
#   define CDS_TSAN_ANNOTATE_HAPPENS_AFTER(addr)    AnnotateHappensAfter(__FILE__, __LINE__, reinterpret_cast<void*>(addr))

#   define CDS_TSAN_ANNOTATE_IGNORE_READS_BEGIN     AnnotateIgnoreReadsBegin(__FILE__, __LINE__)
#   define CDS_TSAN_ANNOTATE_IGNORE_READS_END       AnnotateIgnoreReadsEnd(__FILE__, __LINE__)
#   define CDS_TSAN_ANNOTATE_IGNORE_WRITES_BEGIN    AnnotateIgnoreWritesBegin(__FILE__, __LINE__)
#   define CDS_TSAN_ANNOTATE_IGNORE_WRITES_END      AnnotateIgnoreWritesEnd(__FILE__, __LINE__)
#   define CDS_TSAN_ANNOTATE_IGNORE_RW_BEGIN        \
                                                    CDS_TSAN_ANNOTATE_IGNORE_READS_BEGIN; \
                                                    CDS_TSAN_ANNOTATE_IGNORE_WRITES_BEGIN
#   define CDS_TSAN_ANNOTATE_IGNORE_RW_END          \
                                                    CDS_TSAN_ANNOTATE_IGNORE_WRITES_END;\
                                                    CDS_TSAN_ANNOTATE_IGNORE_READS_END
#   define CDS_TSAN_ANNOTATE_NEW_MEMORY( addr, sz ) AnnotateNewMemory( __FILE__, __LINE__, reinterpret_cast<void *>(addr), sz )

// Publish/unpublish - DEPRECATED
#if 0
#   define CDS_TSAN_ANNOTATE_PUBLISH_MEMORY_RANGE( addr, sz ) AnnotatePublishMemoryRange( __FILE__, __LINE__, reinterpret_cast<void *>(addr), sz )
#   define CDS_TSAN_ANNOTATE_UNPUBLISH_MEMORY_RANGE( addr, sz ) AnnotateUnpublishMemoryRange( __FILE__, __LINE__, reinterpret_cast<void *>(addr), sz )
#endif

#   define CDS_TSAN_ANNOTATE_MUTEX_CREATE( addr )    AnnotateRWLockCreate( __FILE__, __LINE__, reinterpret_cast<void *>(addr))
#   define CDS_TSAN_ANNOTATE_MUTEX_DESTROY( addr )   AnnotateRWLockDestroy( __FILE__, __LINE__, reinterpret_cast<void *>(addr))
        // must be called after actual acquire
#   define CDS_TSAN_ANNOTATE_MUTEX_ACQUIRED( addr )  AnnotateRWLockAcquired( __FILE__, __LINE__, reinterpret_cast<void *>(addr), 1 )
        // must be called before actual release
#   define CDS_TSAN_ANNOTATE_MUTEX_RELEASED( addr )  AnnotateRWLockReleased( __FILE__, __LINE__, reinterpret_cast<void *>(addr), 1 )

    // provided by TSan
    extern "C" {
        void AnnotateHappensBefore(const char *f, int l, void *addr);
        void AnnotateHappensAfter(const char *f, int l, void *addr);

        void AnnotateIgnoreReadsBegin(const char *f, int l);
        void AnnotateIgnoreReadsEnd(const char *f, int l);
        void AnnotateIgnoreWritesBegin(const char *f, int l);
        void AnnotateIgnoreWritesEnd(const char *f, int l);

#if 0
        void AnnotatePublishMemoryRange( const char *f, int l, void * mem, size_t size );
        void AnnotateUnpublishMemoryRange( const char *f, int l, void * addr, size_t size );
#endif
        void AnnotateNewMemory( const char *f, int l, void * mem, size_t size );

        void AnnotateRWLockCreate( const char *f, int l, void* m );
        void AnnotateRWLockDestroy( const char *f, int l, void* m );
        void AnnotateRWLockAcquired( const char *f, int l, void *m, long is_w );
        void AnnotateRWLockReleased( const char *f, int l, void *m, long is_w );
    }

#else // CDS_THREAD_SANITIZER_ENABLED

#   define CDS_TSAN_ANNOTATE_HAPPENS_BEFORE(addr)
#   define CDS_TSAN_ANNOTATE_HAPPENS_AFTER(addr)

#   define CDS_TSAN_ANNOTATE_IGNORE_READS_BEGIN
#   define CDS_TSAN_ANNOTATE_IGNORE_READS_END
#   define CDS_TSAN_ANNOTATE_IGNORE_WRITES_BEGIN
#   define CDS_TSAN_ANNOTATE_IGNORE_WRITES_END
#   define CDS_TSAN_ANNOTATE_IGNORE_RW_BEGIN
#   define CDS_TSAN_ANNOTATE_IGNORE_RW_END

#if 0
#   define CDS_TSAN_ANNOTATE_PUBLISH_MEMORY_RANGE( addr, sz )
#   define CDS_TSAN_ANNOTATE_UNPUBLISH_MEMORY_RANGE( addr, sz )
#endif
#   define CDS_TSAN_ANNOTATE_NEW_MEMORY( addr, sz )

#   define CDS_TSAN_ANNOTATE_MUTEX_CREATE( addr )
#   define CDS_TSAN_ANNOTATE_MUTEX_DESTROY( addr )
#   define CDS_TSAN_ANNOTATE_MUTEX_ACQUIRED( addr )
#   define CDS_TSAN_ANNOTATE_MUTEX_RELEASED( addr )

#endif

//@endcond
#endif  // #ifndef CDSLIB_COMPILER_FEATURE_TSAN_H
