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

#include <cds_test/ext_gtest.h>
#include <cds/init.h>
#include <cds/container/ConcurrentHopscotchHashSet.h>
#include <cds_test/fixture.h>
#include <iostream>

namespace cds_test {
    /*static*/ std::random_device fixture::random_dev_;
    /*static*/ std::mt19937 fixture::random_gen_( random_dev_() );
} // namespace cds_test


using namespace std;


ConcurrentHopscotchHashSet<int, int> *obj;
void testInt() {
	obj = new ConcurrentHopscotchHashSet<int, int>();
	int key1 = 0;
	int val1 = 100;
	int key2 = 1;
	int val2 = 200;
	int key3 = 1025;
	int val3 = 300;

	obj->add(&key1, &val1);
	cout << "Add 1 complete" << endl;
	obj->add(&key2, &val2);
	cout << "Add 2 completes" << endl;
	obj->add(&key3, &val3);
	cout << "Add 3 completes" << endl;

	if (obj->contains(&key1))
		cout << "Key 1 present" << endl;
	if (obj->contains(&key2))
		cout << "Key 2 present" << endl;

	if (obj->contains(&key3))
		cout << "Key 3 present" << endl;

}

//----------------------------------------

void* adding() {
	int key = rand() & 1048575;
	int value = rand() & 1048575;
	if(!obj->add(&key, &value))
		throw exception("Fuuuuu!!!");
}

void testIntAddParallel() {
	obj = new ConcurrentHopscotchHashSet<int, int>();
	pthread_t threads[1000];
	for (int r = 0; r<1000; r++) {
		pthread_create(&threads[r], NULL, adding);
	}
	for (int r = 0; r<1000; r++) {
		pthread_join(threads[r], NULL);
	}
}

//-----------------------------------------------

void* contains(void * arg) {
	int *data = reinterpret_cast<int*>(arg);
	if (!obj->contains(data))
		throw exception("Fuuuuu!!!");
}

void testIntContainsParallel() {
	obj = new ConcurrentHopscotchHashSet<int, int>();
	for (int r = 0; r<1000; r++) {
		obj->add(&r, &r);
	}
	pthread_t threads[1000];
	for (int r = 0; r<1000; r++) {
		pthread_create(&threads[r], NULL, contains, new int(r));
	}
	for (int r = 0; r<1000; r++) {
		pthread_join(threads[r], NULL);
	}
}

//-------------------------

void* removing(void * arg) {
	int *data = reinterpret_cast<int*>(arg);
	if (obj->remove(data) == NULL)
		throw exception("Fuuuuu!!!");
}

void testIntRemoveParallel() {
	obj = new ConcurrentHopscotchHashSet<int, int>();
	for (int r = 0; r<1000; r++) {
		obj->add(&r, &r);
	}
	pthread_t threads[1000];
	for (int r = 0; r<1000; r++) {
		pthread_create(&threads[r], NULL, removing, new int(r));
	}
	for (int r = 0; r<1000; r++) {
		pthread_join(threads[r], NULL);
	}
}

//-------------------------

void* getting(void * arg) {
	int *data = reinterpret_cast<int*>(arg);
	int* res = obj->get(data);
	if (res == NULL)
		throw exception("Bad return!!!");

	if (*res != *data) {
		throw exception("Not equals!!!");
	}
}

void testIntGetParallel() {
	obj = new ConcurrentHopscotchHashSet<int, int>();
	for (int r = 0; r<1000; r++) {
		obj->add(&r, &r);
	}
	pthread_t threads[1000];
	for (int r = 0; r<1000; r++) {
		pthread_create(&threads[r], NULL, getting, new int(r));
	}
	for (int r = 0; r<1000; r++) {
		pthread_join(threads[r], NULL);
	}
}

int main( int argc, char **argv )
{
	testInt();
	testIntAddParallel();
	testIntContainsParallel();
	testIntGetParallel();
	testIntRemoveParallel();
    int result;
    cds::Initialize();
    {
        ::testing::InitGoogleTest( &argc, argv );
        result =  RUN_ALL_TESTS();
    }
    cds::Terminate();
    return result;
}
