//$$CDS-header$$

#ifndef CDSLIB_COMPILER_FEATURE_TSAN_H
#define CDSLIB_COMPILER_FEATURE_TSAN_H

// Thread Sanitizer annotations.
// From https://groups.google.com/d/msg/thread-sanitizer/SsrHB7FTnTk/mNTGNLQj-9cJ

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

    // provided by TSan
    extern "C" {
        void AnnotateHappensBefore(const char *f, int l, void *addr);
        void AnnotateHappensAfter(const char *f, int l, void *addr);

        void AnnotateIgnoreReadsBegin(const char *f, int l);
        void AnnotateIgnoreReadsEnd(const char *f, int l);
        void AnnotateIgnoreWritesBegin(const char *f, int l);
        void AnnotateIgnoreWritesEnd(const char *f, int l);
    }
#else
#   define CDS_TSAN_ANNOTATE_HAPPENS_BEFORE(addr)
#   define CDS_TSAN_ANNOTATE_HAPPENS_AFTER(addr)

#   define CDS_TSAN_ANNOTATE_IGNORE_READS_BEGIN
#   define CDS_TSAN_ANNOTATE_IGNORE_READS_END
#   define CDS_TSAN_ANNOTATE_IGNORE_WRITES_BEGIN
#   define CDS_TSAN_ANNOTATE_IGNORE_WRITES_END
#   define CDS_TSAN_ANNOTATE_IGNORE_RW_BEGIN
#   define CDS_TSAN_ANNOTATE_IGNORE_RW_END
#endif

#endif  // #ifndef CDSLIB_COMPILER_FEATURE_TSAN_H
