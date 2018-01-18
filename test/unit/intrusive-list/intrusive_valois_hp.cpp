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

#include "test_intrusive_list_hp.h"
#include <cds/intrusive/valois_list_hp.h>
#include <thread>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveValoisList_HP : public cds_test::intrusive_list_hp
    {
    public:
        typedef cds_test::intrusive_list_hp::base_item< ci::valois_list::node< gc_type>> base_item;
        typedef cds_test::intrusive_list_hp::member_item< ci::valois_list::node< gc_type>> member_item;

    protected:
        void SetUp()
        {
            struct traits: public ci::valois_list::traits{};

            typedef ci::ValoisList< gc_type, base_item, traits > list_type;

            // +1 - for guarded_ptr
            cds::gc::hp::GarbageCollector::Construct( list_type::c_nHazardPtrCount + 1, 1, 16 );

            cds::threading::Manager::attachThread();

        }

        void TearDown()
        {

            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };

    template <typename List>
    void test_simple_list(List& list)
    {
        std::cout << "simple test started " << std::endl;

        list.append_in_first(3);
        list.append_in_position(10000,10);

        std::cout << "search " << list.search(10000) << std::endl;

        list.deleted(50);

        list.print_all_by_iterator();
    }

    template <typename List>
    void test_list(List& list)
    {
        /**
         * 1) testing method is empty
         * 2) test adding 10 element in the container
         * 3) test comtainig 10 element on the container
         * 4) test deleting element in the containing
         */

        // test empty method();
        ASSERT_TRUE( list.empty());

        static const size_t nSize = 20;
        typedef typename List::value_type value_type;

        // insert and contains method
        for ( int i = 0; i <= nSize; i++ ) {
            int * index = new int32_t(i);
            ASSERT_FALSE(list.contains(*index));
            list.insert(*index);
            ASSERT_TRUE( list.contains(*index));
            ASSERT_FALSE( list.empty());

        }

        // test adding in
        for ( int i = 0; i <= nSize; i++ ) {
            ASSERT_TRUE( list.contains(i));
        }

        // delete and contains method
        for ( int i = 0; i <= nSize; i++ ) {
            ASSERT_TRUE( list.contains(i));
            list.erase(i);
            ASSERT_FALSE(list.contains(i));
        }
        // test empty method();
        ASSERT_TRUE( list.empty());

    }

    template <typename List>
    void revert_test_list(List& list){
        ASSERT_TRUE( list.empty());

        static const size_t nSize = 20;
        typedef typename List::value_type value_type;

        // insert and contains method
        for ( int i = nSize; i >= 0 ; i-- ) {
            int * index = new int32_t(i);
            ASSERT_FALSE(list.contains(*index));
            list.insert(*index);
            ASSERT_TRUE( list.contains(*index));
            ASSERT_FALSE( list.empty());
        }

        // test adding in
        for ( int i = 0; i <= nSize ; i++ ) {
            ASSERT_TRUE( list.contains(i));
        }

        // delete and contains method
        for ( int i = 0; i <= nSize ; i++ ) {
            ASSERT_TRUE( list.contains(i));
            list.erase(i);
            ASSERT_FALSE(list.contains(i));
        }
        // test empty method();
        ASSERT_TRUE( list.empty());
    }


    template <typename List>
    void random_test_list(List& list){
        int items[10] = {4,7,6,8,2,9,3,1,0,5};

        //insert
        for(auto i : items){
            int * index = new int32_t(i);
            ASSERT_FALSE( list.contains(i));
            ASSERT_TRUE(list.insert(*index));
            ASSERT_TRUE( list.contains(i));
        }

        //contains
        for(auto i : items){
            ASSERT_TRUE(list.contains(i));
        }

        //delete
        for ( int i = 0; i <= 9 ; i++ ) {
            ASSERT_TRUE( list.contains(i));
            list.erase(i);
            ASSERT_FALSE( list.contains(i));
        }
    }

    struct traits: public ci::valois_list::traits{};
    /*typedef ci::ValoisList< gc_type, int, traits > list_stress_type;*/
    typedef ci::ValoisList< gc_type, int, traits > list_type;


    struct val_and_id{
        std::thread::id thread_id;
        int value;

        val_and_id(const std::thread::id & thread__id, const int & number){
            thread_id = thread__id;
            value = number;
        }

        bool operator == (const val_and_id & rhs)const {
            return rhs.value == this->value && rhs.thread_id == this->thread_id;
        }

        bool operator < (const val_and_id & rhs)const {
            return rhs.value < this->value;
        }
    };
    typedef ci::ValoisList< gc_type, val_and_id, traits > list_stress_type;

    template <class List>
    void complex_insdel_thread(List& list){
        cds::threading::Manager::attachThread();
        std::thread::id this_id = std::this_thread::get_id();
        std::cout << this_id << std::endl;
        for (int key=0; key < 10000; ++key ) {
            val_and_id  * input = new val_and_id(this_id,key);
            list.insert(* input);
            list.erase(* input);
        }
        cds::threading::Manager::detachThread();
    }
    void complex_insdel_test(){
        std::cout << "complex test started" << std::endl;
        list_stress_type lst;

        std::thread::id this_id = std::this_thread::get_id();
        std::cout << "main threads " << this_id << std::endl;

        std::thread thr(complex_insdel_thread<list_stress_type>,std::ref(lst));
        std::thread thr1(complex_insdel_thread<list_stress_type>,std::ref(lst));
        std::thread thr2(complex_insdel_thread<list_stress_type>,std::ref(lst));
        std::thread thr3(complex_insdel_thread<list_stress_type>,std::ref(lst));
        std::thread thr4(complex_insdel_thread<list_stress_type>,std::ref(lst));
        std::thread thr5(complex_insdel_thread<list_stress_type>,std::ref(lst));
        std::thread thr6(complex_insdel_thread<list_stress_type>,std::ref(lst));
        std::thread thr7(complex_insdel_thread<list_stress_type>,std::ref(lst));
        std::thread thr8(complex_insdel_thread<list_stress_type>,std::ref(lst));
        std::thread thr9(complex_insdel_thread<list_stress_type>,std::ref(lst));

        thr.join();
        thr1.join();
        thr2.join();
        thr3.join();
        thr4.join();
        thr5.join();
        thr6.join();
        thr7.join();
        thr8.join();
        thr9.join();
        std::cout << "Complex test finished" << std::endl;
    }

    TEST_F( IntrusiveValoisList_HP, base_hook )
    {

    struct traits: public ci::valois_list::traits{};


        list_type l2;
        test_list(l2);

        list_type l3;
        revert_test_list(l3);

        list_type l4;
        random_test_list(l4);

        complex_insdel_test();

    }

} // namespace
