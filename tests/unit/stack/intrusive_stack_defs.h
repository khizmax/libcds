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

#ifndef CDSUNIT_INTRUSIVE_STACK_DEFS_H
#define CDSUNIT_INTRUSIVE_STACK_DEFS_H

#define CDSUNIT_DECLARE_TreiberStack \
    TEST_CASE( Treiber_HP, cds::intrusive::treiber_stack::node< cds::gc::HP >        ) \
    TEST_CASE( Treiber_HP_seqcst, cds::intrusive::treiber_stack::node< cds::gc::HP > ) \
    /*TEST_CASE( Treiber_HP_yield, cds::intrusive::treiber_stack::node< cds::gc::HP >  )*/ \
    TEST_CASE( Treiber_HP_pause, cds::intrusive::treiber_stack::node< cds::gc::HP >  ) \
    TEST_CASE( Treiber_HP_exp, cds::intrusive::treiber_stack::node< cds::gc::HP >    ) \
    TEST_CASE( Treiber_HP_stat, cds::intrusive::treiber_stack::node< cds::gc::HP >   ) \
    TEST_CASE( Treiber_DHP, cds::intrusive::treiber_stack::node< cds::gc::DHP >       ) \
    /*TEST_CASE( Treiber_DHP_yield, cds::intrusive::treiber_stack::node< cds::gc::DHP > )*/ \
    TEST_CASE( Treiber_DHP_pause, cds::intrusive::treiber_stack::node< cds::gc::DHP > ) \
    TEST_CASE( Treiber_DHP_exp, cds::intrusive::treiber_stack::node< cds::gc::DHP >   ) \
    TEST_CASE( Treiber_DHP_stat, cds::intrusive::treiber_stack::node< cds::gc::DHP >  )

#define CDSUNIT_TEST_TreiberStack \
    CPPUNIT_TEST( Treiber_HP        ) \
    CPPUNIT_TEST( Treiber_HP_seqcst ) \
    /*CPPUNIT_TEST( Treiber_HP_yield  )*/ \
    CPPUNIT_TEST( Treiber_HP_pause  ) \
    CPPUNIT_TEST( Treiber_HP_exp    ) \
    CPPUNIT_TEST( Treiber_HP_stat   ) \
    CPPUNIT_TEST( Treiber_DHP       ) \
    /*CPPUNIT_TEST( Treiber_DHP_yield )*/ \
    CPPUNIT_TEST( Treiber_DHP_pause ) \
    CPPUNIT_TEST( Treiber_DHP_exp   ) \
    CPPUNIT_TEST( Treiber_DHP_stat  )

#define CDSUNIT_DECLARE_EliminationStack \
    TEST_ELIMINATION( Elimination_HP, cds::intrusive::treiber_stack::node< cds::gc::HP >        ) \
    TEST_ELIMINATION( Elimination_HP_2ms, cds::intrusive::treiber_stack::node< cds::gc::HP >    ) \
    TEST_ELIMINATION( Elimination_HP_2ms_stat, cds::intrusive::treiber_stack::node< cds::gc::HP >) \
    TEST_ELIMINATION( Elimination_HP_5ms, cds::intrusive::treiber_stack::node< cds::gc::HP >    ) \
    TEST_ELIMINATION( Elimination_HP_5ms_stat, cds::intrusive::treiber_stack::node< cds::gc::HP >) \
    TEST_ELIMINATION( Elimination_HP_10ms, cds::intrusive::treiber_stack::node< cds::gc::HP >    ) \
    TEST_ELIMINATION( Elimination_HP_10ms_stat, cds::intrusive::treiber_stack::node< cds::gc::HP >) \
    TEST_ELIMINATION( Elimination_HP_seqcst, cds::intrusive::treiber_stack::node< cds::gc::HP > ) \
    /*TEST_ELIMINATION( Elimination_HP_yield, cds::intrusive::treiber_stack::node< cds::gc::HP >  )*/ \
    TEST_ELIMINATION( Elimination_HP_pause, cds::intrusive::treiber_stack::node< cds::gc::HP >  ) \
    TEST_ELIMINATION( Elimination_HP_exp, cds::intrusive::treiber_stack::node< cds::gc::HP >    ) \
    TEST_ELIMINATION( Elimination_HP_stat, cds::intrusive::treiber_stack::node< cds::gc::HP >   ) \
    TEST_ELIMINATION( Elimination_HP_dyn, cds::intrusive::treiber_stack::node< cds::gc::HP >    ) \
    TEST_ELIMINATION( Elimination_HP_dyn_stat, cds::intrusive::treiber_stack::node< cds::gc::HP >) \
    TEST_ELIMINATION( Elimination_DHP, cds::intrusive::treiber_stack::node< cds::gc::DHP >       ) \
    TEST_ELIMINATION( Elimination_DHP_2ms, cds::intrusive::treiber_stack::node< cds::gc::DHP >    ) \
    TEST_ELIMINATION( Elimination_DHP_2ms_stat, cds::intrusive::treiber_stack::node< cds::gc::DHP >) \
    TEST_ELIMINATION( Elimination_DHP_5ms, cds::intrusive::treiber_stack::node< cds::gc::DHP >    ) \
    TEST_ELIMINATION( Elimination_DHP_5ms_stat, cds::intrusive::treiber_stack::node< cds::gc::DHP >) \
    TEST_ELIMINATION( Elimination_DHP_10ms, cds::intrusive::treiber_stack::node< cds::gc::DHP >    ) \
    TEST_ELIMINATION( Elimination_DHP_10ms_stat, cds::intrusive::treiber_stack::node< cds::gc::DHP >) \
    TEST_ELIMINATION( Elimination_DHP_seqcst, cds::intrusive::treiber_stack::node< cds::gc::DHP > ) \
    /*TEST_ELIMINATION( Elimination_DHP_yield, cds::intrusive::treiber_stack::node< cds::gc::DHP > )*/ \
    TEST_ELIMINATION( Elimination_DHP_pause, cds::intrusive::treiber_stack::node< cds::gc::DHP > ) \
    TEST_ELIMINATION( Elimination_DHP_exp, cds::intrusive::treiber_stack::node< cds::gc::DHP >   ) \
    TEST_ELIMINATION( Elimination_DHP_stat, cds::intrusive::treiber_stack::node< cds::gc::DHP >  ) \
    TEST_ELIMINATION( Elimination_DHP_dyn, cds::intrusive::treiber_stack::node< cds::gc::DHP >   ) \
    TEST_ELIMINATION( Elimination_DHP_dyn_stat, cds::intrusive::treiber_stack::node< cds::gc::DHP >)

#define CDSUNIT_TEST_EliminationStack \
    CPPUNIT_TEST( Elimination_HP        ) \
    CPPUNIT_TEST( Elimination_HP_2ms    ) \
    CPPUNIT_TEST( Elimination_HP_2ms_stat) \
    CPPUNIT_TEST( Elimination_HP_5ms    ) \
    CPPUNIT_TEST( Elimination_HP_5ms_stat) \
    CPPUNIT_TEST( Elimination_HP_10ms    ) \
    CPPUNIT_TEST( Elimination_HP_10ms_stat) \
    CPPUNIT_TEST( Elimination_HP_seqcst ) \
    /*CPPUNIT_TEST( Elimination_HP_yield  )*/ \
    CPPUNIT_TEST( Elimination_HP_pause  ) \
    CPPUNIT_TEST( Elimination_HP_exp    ) \
    CPPUNIT_TEST( Elimination_HP_stat   ) \
    CPPUNIT_TEST( Elimination_HP_dyn    ) \
    CPPUNIT_TEST( Elimination_HP_dyn_stat) \
    CPPUNIT_TEST( Elimination_DHP        ) \
    CPPUNIT_TEST( Elimination_DHP_seqcst ) \
    CPPUNIT_TEST( Elimination_DHP_2ms    ) \
    CPPUNIT_TEST( Elimination_DHP_2ms_stat) \
    CPPUNIT_TEST( Elimination_DHP_5ms    ) \
    CPPUNIT_TEST( Elimination_DHP_5ms_stat) \
    CPPUNIT_TEST( Elimination_DHP_10ms    ) \
    CPPUNIT_TEST( Elimination_DHP_10ms_stat) \
    /*CPPUNIT_TEST( Elimination_DHP_yield )*/ \
    CPPUNIT_TEST( Elimination_DHP_pause ) \
    CPPUNIT_TEST( Elimination_DHP_exp   ) \
    CPPUNIT_TEST( Elimination_DHP_stat  ) \
    CPPUNIT_TEST( Elimination_DHP_dyn   ) \
    CPPUNIT_TEST( Elimination_DHP_dyn_stat)

#define CDSUNIT_DECLARE_FCStack \
    TEST_FCSTACK( FCStack_slist, boost::intrusive::slist_base_hook<> ) \
    TEST_FCSTACK( FCStack_slist_stat, boost::intrusive::slist_base_hook<> ) \
    TEST_FCSTACK( FCStack_slist_elimination, boost::intrusive::slist_base_hook<> ) \
    TEST_FCSTACK( FCStack_slist_elimination_stat, boost::intrusive::slist_base_hook<> ) \
    TEST_FCSTACK( FCStack_slist_mutex_stat, boost::intrusive::slist_base_hook<> ) \
    TEST_FCSTACK( FCStack_slist_mutex_elimination, boost::intrusive::slist_base_hook<> ) \
    TEST_FCSTACK( FCStack_slist_mutex_elimination_stat, boost::intrusive::slist_base_hook<> ) \
    TEST_FCSTACK( FCStack_list, boost::intrusive::list_base_hook<> ) \
    TEST_FCSTACK( FCStack_list_stat, boost::intrusive::list_base_hook<> ) \
    TEST_FCSTACK( FCStack_list_elimination, boost::intrusive::list_base_hook<> ) \
    TEST_FCSTACK( FCStack_list_elimination_stat, boost::intrusive::list_base_hook<> ) \
    TEST_FCSTACK( FCStack_list_mutex_stat, boost::intrusive::list_base_hook<> ) \
    TEST_FCSTACK( FCStack_list_mutex_elimination, boost::intrusive::list_base_hook<> ) \
    TEST_FCSTACK( FCStack_list_mutex_elimination_stat, boost::intrusive::list_base_hook<> )

#define CDSUNIT_TEST_FCStack \
    CPPUNIT_TEST( FCStack_slist ) \
    CPPUNIT_TEST( FCStack_slist_stat ) \
    CPPUNIT_TEST( FCStack_slist_elimination ) \
    CPPUNIT_TEST( FCStack_slist_elimination_stat ) \
    CPPUNIT_TEST( FCStack_slist_mutex_stat ) \
    CPPUNIT_TEST( FCStack_slist_mutex_elimination ) \
    CPPUNIT_TEST( FCStack_slist_mutex_elimination_stat ) \
    CPPUNIT_TEST( FCStack_list ) \
    CPPUNIT_TEST( FCStack_list_stat ) \
    CPPUNIT_TEST( FCStack_list_elimination ) \
    CPPUNIT_TEST( FCStack_list_elimination_stat ) \
    CPPUNIT_TEST( FCStack_list_mutex_stat ) \
    CPPUNIT_TEST( FCStack_list_mutex_elimination ) \
    CPPUNIT_TEST( FCStack_list_mutex_elimination_stat )


#define CDSUNIT_DECLARE_StdStack \
    TEST_STDSTACK( StdStack_Deque_Mutex  ) \
    TEST_STDSTACK( StdStack_Deque_Spin   ) \
    TEST_STDSTACK( StdStack_Vector_Mutex ) \
    TEST_STDSTACK( StdStack_Vector_Spin  ) \
    TEST_STDSTACK( StdStack_List_Mutex   ) \
    TEST_STDSTACK( StdStack_List_Spin    )

#define CDSUNIT_TEST_StdStack \
    CPPUNIT_TEST( StdStack_Deque_Mutex  ) \
    CPPUNIT_TEST( StdStack_Deque_Spin   ) \
    CPPUNIT_TEST( StdStack_Vector_Mutex ) \
    CPPUNIT_TEST( StdStack_Vector_Spin  ) \
    CPPUNIT_TEST( StdStack_List_Mutex   ) \
    CPPUNIT_TEST( StdStack_List_Spin    )

#endif // #ifndef CDSUNIT_INTRUSIVE_STACK_DEFS_H
