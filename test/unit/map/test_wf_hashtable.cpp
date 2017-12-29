#ifndef CDSLIB_CONTAINER_EAWfPAD_HASHTABLE_H
#define CDSLIB_CONTAINER_EAWfPAD_HASHTABLE_H

#include <cds/init.h>
#include <cds/container/wf_hashtable.h>

#include <iostream>

using namespace std;

pthread_mutex_t myMutex[2];
pthread_cond_t cond;
volatile bool isAssignStarted = false;

cds::container::concurrent_hopscotch_hash_set<int, int> *obj;
void testfunc() {
	obj = new cds::container::concurrent_hopscotch_hash_set<int, int>(10)->getProcess();
	int key1 = 0;
	int val1 = 100;
	int key2 = 1;
	int val2 = 200;
	int key3 = 1025;
	int val3 = 300;
	if(!obj->insert(key1, &val1))
		throw exception("Fuuuuu!!!");
	}	
	cout << "Add 1 complete" << endl;
	obj->assign(key1, &val2);
	if(*obj->find(r)!=val2)
		throw exception("Fuuuuu!!!");
	}
	cout << "Assign complete" << endl;
	if(!obj->insert(key2, &val2))
		throw exception("Fuuuuu!!!");
	}
	cout << "Add 2 complete" << endl;
	if(!obj->insert(key3, &val3))
		throw exception("Fuuuuu!!!");
	}
	cout << "Add 3 complete" << endl;

	if (obj->del(key1))
		cout << "Del 1 complete" << endl;
	if (obj->del(key2))
		cout << "Del 2 complete" << endl;
	if (obj->del(key3))
		cout << "Del 3 complete" << endl;

}

//----------------------------------------

void* adding(void * arg, void * arg1) {
	int *data = reinterpret_cast<int*>(arg);
	int *data1 = reinterpret_cast<int*>(arg1);
	int value = rand() & 1048575;
	for (int r = *data; r<*data1; r++){
	if(!obj->add(r, &value))
		throw exception("Fuuuuu!!!");
	}	
	
}



void* removing(void * arg, void * arg1) {
	int *data = reinterpret_cast<int*>(arg);
	int *data1 = reinterpret_cast<int*>(arg1);
	for (int r = *data; r<*data1; r++){
	pthread_mutex_lock(&myMutex[*data/500]);
	if (!obj->del(r))
		throw exception("Fuuuuu!!!");
	}
	pthread_mutex_unlock(&myMutex[*data/500]);
}


void* assging(void * arg, void * arg1) {
	int *data = reinterpret_cast<int*>(arg);
	int value = rand() & 1048575;
	for (int r = *data; r<*data1; r++){
	pthread_mutex_lock(&myMutex[*data/500]);
	obj->assign(r,value);
	if(*obj->find(r)!=value)
		throw exception("Fuuuuu!!!");
	}
	pthread_mutex_unlock(&myMutex[*data/500]);
}

void testParallel() {
	obj = new cds::container::concurrent_hopscotch_hash_set<int, int>(6);
	pthread_t threads[6];
	for (int r = 0; r<2; r++) {
		pthread_create(&threads[2*r], NULL, adding, new int (500*r),new int (500*(r+1)));
		pthread_create(&threads[2*r+1], NULL, assign, new int (500*r),new int (500*(r+1)));
		pthread_create(&threads[3*r+2], NULL, removing, new int (500*r),new int (500*(r+1)));
	}
	for (int r = 0; r<6; r++) {
		pthread_join(threads[r], NULL);
	}
}

int main( int argc, char **argv )
{
	testfunc();
	testParallel();
    int result;
    cds::Initialize();
    {
        ::testing::InitGoogleTest( &argc, argv );
        result =  RUN_ALL_TESTS();
    }
    cds::Terminate();
    return result;
}
#endif
