#include <cds/init.h>  //cds::Initialize и cds::Terminate
#include <cds/gc/hp.h> //cds::gc::HP (Hazard Pointer)
#include <iostream> //cds::gc::HP (Hazard Pointer)
#include <cds/container/optimistic_queue.h> // cds::gc::HP (Hazard Pointer)
#include <cds/container/hamt.h> // cds::gc::HP (Hazard Pointer)

using namespace std;

int main() {
    cds::Initialize();
    {
        cds::gc::HP hpGC;
        cds::threading::Manager::attachThread();

        for (int j = 0; j < 10000000; j++) {
            cds::container::Hamt<cds::gc::HP, int, int> hamt;
            int count = 10000;

            cout << "start\n";
            int thread_count = 10;
            vector<pthread_t> thread(thread_count);
            vector<vector<void *>> attr(thread_count);

            for (int i = 0; i < attr.size(); i++) {
                attr[i] = {&hamt, new int(i), new int(count / thread_count)};
            }
            for (int i = 0; i < thread.size(); i++) {
                pthread_create(&thread[i], nullptr, [](void *args) -> void * {
                    cds::threading::Manager::attachThread();
                    auto *hamt = (cds::container::Hamt<cds::gc::HP, int, int> *) (*static_cast<vector<void *> *>(args))[0];
                    int *id = (int *) (*static_cast<vector<void *> *>(args))[1];
                    int *averageIterationCount = (int *) (*static_cast<vector<void *> *>(args))[2];
                    for (int i = *id * (*averageIterationCount); i < (*id + 1) * (*averageIterationCount); i++) {
                        hamt->insert(i,i);
                    }
                    cds::threading::Manager::detachThread();
                    pthread_exit(nullptr);
                }, &attr[i]);

            }

            for (unsigned long i: thread) {
                pthread_join(i, nullptr);
            }

            for (int i = 0; i < thread_count; i++) {
                delete (int *) attr[i][1];
                delete (int *) attr[i][2];
            }
            cout << "finished\n";
//            for (int i = 0; i < count; i++) {
//                assert(hamt.lookup(i).value == 0);
//            }
        }
    }


//    cds::Terminate();

}