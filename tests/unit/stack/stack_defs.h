//$$CDS-header$$

#ifndef __CDSUNIT_STACK_DEFS_H
#define __CDSUNIT_STACK_DEFS_H

#define CDSUNIT_DECLARE_TreiberStack \
    TEST_CASE( Treiber_HP        ) \
    TEST_CASE( Treiber_HP_seqcst ) \
    /*TEST_CASE( Treiber_HP_yield  )*/ \
    TEST_CASE( Treiber_HP_pause  ) \
    TEST_CASE( Treiber_HP_exp    ) \
    TEST_CASE( Treiber_HP_stat   ) \
    /*TEST_CASE( Treiber_HRC       )*/ \
    /*TEST_CASE( Treiber_HRC_yield )*/ \
    /*TEST_CASE( Treiber_HRC_pause )*/ \
    /*TEST_CASE( Treiber_HRC_exp   )*/ \
    /*TEST_CASE( Treiber_HRC_stat  )*/ \
    TEST_CASE( Treiber_PTB       ) \
    /*TEST_CASE( Treiber_PTB_yield )*/ \
    TEST_CASE( Treiber_PTB_pause ) \
    TEST_CASE( Treiber_PTB_exp   ) \
    TEST_CASE( Treiber_PTB_stat  )

#define CDSUNIT_TEST_TreiberStack \
    CPPUNIT_TEST( Treiber_HP        ) \
    CPPUNIT_TEST( Treiber_HP_seqcst ) \
    /*CPPUNIT_TEST( Treiber_HP_yield  )*/ \
    CPPUNIT_TEST( Treiber_HP_pause  ) \
    CPPUNIT_TEST( Treiber_HP_exp    ) \
    CPPUNIT_TEST( Treiber_HP_stat   ) \
    /*CPPUNIT_TEST( Treiber_HRC       )*/ \
    /*CPPUNIT_TEST( Treiber_HRC_yield )*/ \
    /*CPPUNIT_TEST( Treiber_HRC_pause )*/ \
    /*CPPUNIT_TEST( Treiber_HRC_exp   )*/ \
    /*CPPUNIT_TEST( Treiber_HRC_stat  )*/ \
    CPPUNIT_TEST( Treiber_PTB       ) \
    /*CPPUNIT_TEST( Treiber_PTB_yield )*/ \
    CPPUNIT_TEST( Treiber_PTB_pause ) \
    CPPUNIT_TEST( Treiber_PTB_exp   ) \
    CPPUNIT_TEST( Treiber_PTB_stat  )

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
    /*TEST_ELIMINATION( Elimination_HRC       )*/ \
    /*TEST_ELIMINATION( Elimination_HRC_yield )*/ \
    /*TEST_ELIMINATION( Elimination_HRC_pause )*/ \
    /*TEST_ELIMINATION( Elimination_HRC_exp   )*/ \
    /*TEST_ELIMINATION( Elimination_HRC_stat  )*/ \
    /*TEST_ELIMINATION( Elimination_HRC_dyn   )*/ \
    /*TEST_ELIMINATION( Elimination_HRC_dyn_stat)*/ \
    TEST_ELIMINATION( Elimination_PTB       ) \
    TEST_ELIMINATION( Elimination_PTB_2ms    ) \
    TEST_ELIMINATION( Elimination_PTB_2ms_stat) \
    TEST_ELIMINATION( Elimination_PTB_5ms    ) \
    TEST_ELIMINATION( Elimination_PTB_5ms_stat) \
    TEST_ELIMINATION( Elimination_PTB_10ms    ) \
    TEST_ELIMINATION( Elimination_PTB_10ms_stat) \
    /*TEST_ELIMINATION( Elimination_PTB_yield )*/ \
    TEST_ELIMINATION( Elimination_PTB_pause ) \
    TEST_ELIMINATION( Elimination_PTB_exp   ) \
    TEST_ELIMINATION( Elimination_PTB_stat  ) \
    TEST_ELIMINATION( Elimination_PTB_dyn   ) \
    TEST_ELIMINATION( Elimination_PTB_dyn_stat)

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
    /*CPPUNIT_TEST( Elimination_HRC       )*/ \
    /*CPPUNIT_TEST( Elimination_HRC_yield )*/ \
    /*CPPUNIT_TEST( Elimination_HRC_pause )*/ \
    /*CPPUNIT_TEST( Elimination_HRC_exp   )*/ \
    /*CPPUNIT_TEST( Elimination_HRC_stat  )*/ \
    /*CPPUNIT_TEST( Elimination_HRC_dyn   )*/ \
    /*CPPUNIT_TEST( Elimination_HRC_dyn_stat)*/ \
    CPPUNIT_TEST( Elimination_PTB       ) \
    CPPUNIT_TEST( Elimination_PTB_2ms    ) \
    CPPUNIT_TEST( Elimination_PTB_2ms_stat) \
    CPPUNIT_TEST( Elimination_PTB_5ms    ) \
    CPPUNIT_TEST( Elimination_PTB_5ms_stat) \
    CPPUNIT_TEST( Elimination_PTB_10ms    ) \
    CPPUNIT_TEST( Elimination_PTB_10ms_stat) \
    /*CPPUNIT_TEST( Elimination_PTB_yield )*/ \
    CPPUNIT_TEST( Elimination_PTB_pause ) \
    CPPUNIT_TEST( Elimination_PTB_exp   ) \
    CPPUNIT_TEST( Elimination_PTB_stat  ) \
    CPPUNIT_TEST( Elimination_PTB_dyn   ) \
    CPPUNIT_TEST( Elimination_PTB_dyn_stat)

#define CDSUNIT_DECLARE_MichaelDeque \
    TEST_BOUNDED( MichaelDequeL_HP          ) \
    TEST_BOUNDED( MichaelDequeL_HP_seqcst   ) \
    TEST_BOUNDED( MichaelDequeL_HP_ic       ) \
    TEST_BOUNDED( MichaelDequeL_HP_exp      ) \
    TEST_BOUNDED( MichaelDequeL_HP_yield    ) \
    TEST_BOUNDED( MichaelDequeL_HP_stat     ) \
    TEST_BOUNDED( MichaelDequeL_PTB         ) \
    TEST_BOUNDED( MichaelDequeL_PTB_seqcst  ) \
    TEST_BOUNDED( MichaelDequeL_PTB_ic      ) \
    TEST_BOUNDED( MichaelDequeL_PTB_exp     ) \
    TEST_BOUNDED( MichaelDequeL_PTB_yield   ) \
    TEST_BOUNDED( MichaelDequeL_PTB_stat    ) \
    TEST_BOUNDED( MichaelDequeR_HP          ) \
    TEST_BOUNDED( MichaelDequeR_HP_seqcst   ) \
    TEST_BOUNDED( MichaelDequeR_HP_ic       ) \
    TEST_BOUNDED( MichaelDequeR_HP_exp      ) \
    TEST_BOUNDED( MichaelDequeR_HP_yield    ) \
    TEST_BOUNDED( MichaelDequeR_HP_stat     ) \
    TEST_BOUNDED( MichaelDequeR_PTB         ) \
    TEST_BOUNDED( MichaelDequeR_PTB_seqcst  ) \
    TEST_BOUNDED( MichaelDequeR_PTB_ic      ) \
    TEST_BOUNDED( MichaelDequeR_PTB_exp     ) \
    TEST_BOUNDED( MichaelDequeR_PTB_yield   ) \
    TEST_BOUNDED( MichaelDequeR_PTB_stat    )

#define CDSUNIT_TEST_MichaelDeque \
    CPPUNIT_TEST( MichaelDequeL_HP          ) \
    /*CPPUNIT_TEST( MichaelDequeL_HP_seqcst   )*/ \
    /*CPPUNIT_TEST( MichaelDequeL_HP_ic       )*/ \
    /*CPPUNIT_TEST( MichaelDequeL_HP_exp      )*/ \
    /*CPPUNIT_TEST( MichaelDequeL_HP_yield    )*/ \
    CPPUNIT_TEST( MichaelDequeL_HP_stat     ) \
    CPPUNIT_TEST( MichaelDequeL_PTB         ) \
    /*CPPUNIT_TEST( MichaelDequeL_PTB_seqcst  )*/ \
    /*CPPUNIT_TEST( MichaelDequeL_PTB_ic      )*/ \
    /*CPPUNIT_TEST( MichaelDequeL_PTB_exp     )*/ \
    /*CPPUNIT_TEST( MichaelDequeL_PTB_yield   )*/ \
    CPPUNIT_TEST( MichaelDequeL_PTB_stat    ) \
    CPPUNIT_TEST( MichaelDequeR_HP          ) \
    /*CPPUNIT_TEST( MichaelDequeR_HP_seqcst   )*/ \
    /*CPPUNIT_TEST( MichaelDequeR_HP_ic       )*/ \
    /*CPPUNIT_TEST( MichaelDequeR_HP_exp      )*/ \
    /*CPPUNIT_TEST( MichaelDequeR_HP_yield    )*/ \
    CPPUNIT_TEST( MichaelDequeR_HP_stat     ) \
    CPPUNIT_TEST( MichaelDequeR_PTB         ) \
    /*CPPUNIT_TEST( MichaelDequeR_PTB_seqcst  )*/ \
    /*CPPUNIT_TEST( MichaelDequeR_PTB_ic      )*/ \
    /*CPPUNIT_TEST( MichaelDequeR_PTB_exp     )*/ \
    /*CPPUNIT_TEST( MichaelDequeR_PTB_yield   )*/ \
    CPPUNIT_TEST( MichaelDequeR_PTB_stat    )

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

#endif // #ifndef __CDSUNIT_STACK_DEFS_H
