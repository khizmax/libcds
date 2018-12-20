#include <pthread.h>
#include <cstdlib>

class ThreadContext
{
public:
    static ThreadContext &get();
    static void prepare(uint64_t num_threads);
    static void assign_context();

    inline uint64_t thread_id()
    {
        return thread_id_;
    }

private:
    static constexpr uint64_t kMaxThreads = 1024;
    static uint64_t global_thread_id_cnt;
    static ThreadContext *contexts[kMaxThreads];
    static pthread_key_t threadcontext_key;

    ThreadContext() {}

    uint64_t thread_id_;
};

uint64_t ThreadContext::global_thread_id_cnt = 0;
pthread_key_t ThreadContext::threadcontext_key;
ThreadContext *ThreadContext::contexts[kMaxThreads];

ThreadContext &ThreadContext::get()
{
    if (pthread_getspecific(threadcontext_key) == NULL)
    {
        assign_context();
    }
    ThreadContext *context = static_cast<ThreadContext *>(
        pthread_getspecific(threadcontext_key));
    return *context;
}

void ThreadContext::assign_context()
{
    uint64_t thread_id = __sync_fetch_and_add(&global_thread_id_cnt, 1);
    if (pthread_setspecific(threadcontext_key, contexts[thread_id]))
    {
        fprintf(stderr, "%s: pthread_setspecific failed\n", __func__);
        exit(EXIT_FAILURE);
    }
}

void ThreadContext::prepare(uint64_t num_threads)
{
    pthread_key_create(&threadcontext_key, NULL);
    for (uint64_t i = 0; i < num_threads; i++)
    {
        ThreadContext *context = new ThreadContext();
        context->thread_id_ = i;
        contexts[i] = context;
    }
}