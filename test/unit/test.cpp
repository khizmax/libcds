#include <cds/init.h>
#include <cds/container/wf_hashtable.h>

#include <iostream>

using namespace std;


cds::container::concurrent_hopscotch_hash_set<int, int> *obj;
void testfunc() {
	obj = new cds::container::concurrent_hopscotch_hash_set<int, int>();
	int key1 = 0;
	int val1 = 100;
	int key2 = 1;
	int val2 = 200;
	int key3 = 1025;
	int val3 = 300;

	obj->insert(key1, &val1);
	cout << "Add 1 complete" << endl;
	obj->insert(key2, &val2);
	cout << "Add 2 completes" << endl;
	obj->insert(key3, &val3);
	cout << "Add 3 completes" << endl;

	if (obj->find(key1))
		cout << "Key 1 present" << endl;
	if (obj->find(key2))
		cout << "Key 2 present" << endl;

	if (obj->find(key3))
		cout << "Key 3 present" << endl;

}

//----------------------------------------

void* adding(void * arg) {
	int *data = reinterpret_cast<int*>(arg);
	int value = rand() & 1048575;
	for (int r = 0; r<data; r++){
	if(!obj->add(r, &value))
		throw exception("Fuuuuu!!!");
	}	
	
}



void* removing(void * arg) {
	 pthread_mutex_lock(&myMutex);
	int *data = reinterpret_cast<int*>(arg);
	for (int r = 0; r<data; r++){
	if (!obj->del(r))
		throw exception("Fuuuuu!!!");
	}
}


void* assging(void * arg, void * arg1) {
	int *data = reinterpret_cast<int*>(arg);
	int value = rand() & 1048575;
	for (int r = 0; r<data; r++){
	obj->assign(r,value);
	if(!obj->find(r))
		throw exception("Fuuuuu!!!");
	}
	
}

void testParallel() {
	obj = new cds::container::concurrent_hopscotch_hash_set<int, int>(6);
	pthread_t threads[6];
	for (int r = 0; r<2; r++) {
		pthread_create(&threads[3*r], NULL, adding, new int (1000));
		pthread_create(&threads[3*r+1], NULL, assign, new int (1000));
		pthread_create(&threads[3*r+2], NULL, removing, new int (1000));
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
