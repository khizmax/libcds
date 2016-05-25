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

#ifndef CDSUNIT_STACK_DEFS_H
#define CDSUNIT_STACK_DEFS_H

#define CDSUNIT_DECLARE_TreiberStack \
    TEST_CASE( Treiber_HP        ) \
    TEST_CASE( Treiber_HP_seqcst ) \
    /*TEST_CASE( Treiber_HP_yield  )*/ \
    TEST_CASE( Treiber_HP_pause  ) \
    TEST_CASE( Treiber_HP_exp    ) \
    TEST_CASE( Treiber_HP_stat   ) \
    TEST_CASE( Treiber_DHP       ) \
    /*TEST_CASE( Treiber_DHP_yield )*/ \
    TEST_CASE( Treiber_DHP_pause ) \
    TEST_CASE( Treiber_DHP_exp   ) \
    TEST_CASE( Treiber_DHP_stat  )

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
    TEST_ELIMINATION( Elimination_HP        ) \
    TEST_ELIMINATION( Elimination_HP_2ms    ) \
    TEST_ELIMINATION( Elimination_HP_2ms_stat) \
    TEST_ELIMINATION( Elimination_HP_5ms    ) \
    TEST_ELIMINATION( Elimination_HP_5ms_stat) \
    TEST_ELIMINATION( Elimination_HP_10ms    ) \
    TEST_ELIMINATION( Elimination_HP_10ms_stat) \
    TEST_ELIMINATION( Elimination_HP_seqcst ) \
    /*TEST_ELIMINATION( Elimination_HP_yield  )*/ \
    TEST_ELIMINATION( Elimination_HP_pause  ) \
    TEST_ELIMINATION( Elimination_HP_exp    ) \
    TEST_ELIMINATION( Elimination_HP_stat   ) \
    TEST_ELIMINATION( Elimination_HP_dyn    ) \
    TEST_ELIMINATION( Elimination_HP_dyn_stat) \
    TEST_ELIMINATION( Elimination_DHP       ) \
    TEST_ELIMINATION( Elimination_DHP_2ms    ) \
    TEST_ELIMINATION( Elimination_DHP_2ms_stat) \
    TEST_ELIMINATION( Elimination_DHP_5ms    ) \
    TEST_ELIMINATION( Elimination_DHP_5ms_stat) \
    TEST_ELIMINATION( Elimination_DHP_10ms    ) \
    TEST_ELIMINATION( Elimination_DHP_10ms_stat) \
    /*TEST_ELIMINATION( Elimination_DHP_yield )*/ \
    TEST_ELIMINATION( Elimination_DHP_pause ) \
    TEST_ELIMINATION( Elimination_DHP_exp   ) \
    TEST_ELIMINATION( Elimination_DHP_stat  ) \
    TEST_ELIMINATION( Elimination_DHP_dyn   ) \
    TEST_ELIMINATION( Elimination_DHP_dyn_stat)

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
    CPPUNIT_TEST( Elimination_DHP       ) \
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
    TEST_CASE( FCStack_deque ) \
    TEST_CASE( FCStack_deque_mutex ) \
    TEST_CASE( FCStack_deque_stat ) \
    TEST_CASE( FCStack_deque_elimination ) \
    TEST_CASE( FCStack_deque_elimination_stat ) \
    TEST_CASE( FCStack_vector ) \
    TEST_CASE( FCStack_vector_mutex ) \
    TEST_CASE( FCStack_vector_stat ) \
    TEST_CASE( FCStack_vector_elimination ) \
    TEST_CASE( FCStack_vector_elimination_stat ) \
    TEST_CASE( FCStack_list ) \
    TEST_CASE( FCStack_list_mutex ) \
    TEST_CASE( FCStack_list_stat ) \
    TEST_CASE( FCStack_list_elimination ) \
    TEST_CASE( FCStack_list_elimination_stat )

#define CDSUNIT_TEST_FCStack \
    CPPUNIT_TEST( FCStack_deque ) \
    CPPUNIT_TEST( FCStack_deque_mutex ) \
    CPPUNIT_TEST( FCStack_deque_stat ) \
    CPPUNIT_TEST( FCStack_deque_elimination ) \
    CPPUNIT_TEST( FCStack_deque_elimination_stat ) \
    CPPUNIT_TEST( FCStack_vector ) \
    CPPUNIT_TEST( FCStack_vector_mutex ) \
    CPPUNIT_TEST( FCStack_vector_stat ) \
    CPPUNIT_TEST( FCStack_vector_elimination ) \
    CPPUNIT_TEST( FCStack_vector_elimination_stat ) \
    CPPUNIT_TEST( FCStack_list ) \
    CPPUNIT_TEST( FCStack_list_mutex ) \
    CPPUNIT_TEST( FCStack_list_stat ) \
    CPPUNIT_TEST( FCStack_list_elimination ) \
    CPPUNIT_TEST( FCStack_list_elimination_stat )


#define CDSUNIT_DECLARE_FCDeque \
    TEST_CASE( FCDequeL_default ) \
    TEST_CASE( FCDequeL_mutex ) \
    TEST_CASE( FCDequeL_stat ) \
    TEST_CASE( FCDequeL_elimination ) \
    TEST_CASE( FCDequeL_elimination_stat ) \
    TEST_CASE( FCDequeR_default ) \
    TEST_CASE( FCDequeR_mutex ) \
    TEST_CASE( FCDequeR_stat ) \
    TEST_CASE( FCDequeR_elimination ) \
    TEST_CASE( FCDequeR_elimination_stat )

#define CDSUNIT_TEST_FCDeque \
    CPPUNIT_TEST( FCDequeL_default ) \
    CPPUNIT_TEST( FCDequeL_mutex ) \
    CPPUNIT_TEST( FCDequeL_stat ) \
    CPPUNIT_TEST( FCDequeL_elimination ) \
    CPPUNIT_TEST( FCDequeL_elimination_stat ) \
    CPPUNIT_TEST( FCDequeR_default ) \
    CPPUNIT_TEST( FCDequeR_mutex ) \
    CPPUNIT_TEST( FCDequeR_stat ) \
    CPPUNIT_TEST( FCDequeR_elimination ) \
    CPPUNIT_TEST( FCDequeR_elimination_stat )


#define CDSUNIT_DECLARE_StdStack \
    TEST_CASE( StdStack_Deque_Mutex  ) \
    TEST_CASE( StdStack_Deque_Spin   ) \
    TEST_CASE( StdStack_Vector_Mutex ) \
    TEST_CASE( StdStack_Vector_Spin  ) \
    TEST_CASE( StdStack_List_Mutex   ) \
    TEST_CASE( StdStack_List_Spin    )

#define CDSUNIT_TEST_StdStack \
    CPPUNIT_TEST( StdStack_Deque_Mutex  ) \
    CPPUNIT_TEST( StdStack_Deque_Spin   ) \
    CPPUNIT_TEST( StdStack_Vector_Mutex ) \
    CPPUNIT_TEST( StdStack_Vector_Spin  ) \
    CPPUNIT_TEST( StdStack_List_Mutex   ) \
    CPPUNIT_TEST( StdStack_List_Spin    )


#define CDSUNIT_DECLARE_WilliamsStack \
    TEST_CASE( WilliamsStack_default) \
	TEST_CASE( WilliamsStack_item_counter)

#define CDSUNIT_TEST_WilliamsStack \
    CPPUNIT_TEST( WilliamsStack_default) \
	CPPUNIT_TEST( WilliamsStack_item_counter)
    

#endif // #ifndef CDSUNIT_STACK_DEFS_H
