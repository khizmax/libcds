//$$CDS-header$$

#ifndef __CDSUNIT_INTRUSIVE_STACK_DEFS_H
#define __CDSUNIT_INTRUSIVE_STACK_DEFS_H

#define CDSUNIT_DECLARE_TreiberStack \
    TEST_CASE( Treiber_HP, cds::intrusive::single_link::node< cds::gc::HP >        ) \
    TEST_CASE( Treiber_HP_seqcst, cds::intrusive::single_link::node< cds::gc::HP > ) \
    /*TEST_CASE( Treiber_HP_yield, cds::intrusive::single_link::node< cds::gc::HP >  )*/ \
    TEST_CASE( Treiber_HP_pause, cds::intrusive::single_link::node< cds::gc::HP >  ) \
    TEST_CASE( Treiber_HP_exp, cds::intrusive::single_link::node< cds::gc::HP >    ) \
    TEST_CASE( Treiber_HP_stat, cds::intrusive::single_link::node< cds::gc::HP >   ) \
    TEST_CASE( Treiber_HRC, cds::intrusive::single_link::node< cds::gc::HRC >       ) \
    /*TEST_CASE( Treiber_HRC_yield, cds::intrusive::single_link::node< cds::gc::HRC > )*/ \
    TEST_CASE( Treiber_HRC_pause, cds::intrusive::single_link::node< cds::gc::HRC > ) \
    TEST_CASE( Treiber_HRC_exp, cds::intrusive::single_link::node< cds::gc::HRC >   ) \
    TEST_CASE( Treiber_HRC_stat, cds::intrusive::single_link::node< cds::gc::HRC >  ) \
    TEST_CASE( Treiber_PTB, cds::intrusive::single_link::node< cds::gc::PTB >       ) \
    /*TEST_CASE( Treiber_PTB_yield, cds::intrusive::single_link::node< cds::gc::PTB > )*/ \
    TEST_CASE( Treiber_PTB_pause, cds::intrusive::single_link::node< cds::gc::PTB > ) \
    TEST_CASE( Treiber_PTB_exp, cds::intrusive::single_link::node< cds::gc::PTB >   ) \
    TEST_CASE( Treiber_PTB_stat, cds::intrusive::single_link::node< cds::gc::PTB >  )

#define CDSUNIT_TEST_TreiberStack \
    CPPUNIT_TEST( Treiber_HP        ) \
    CPPUNIT_TEST( Treiber_HP_seqcst ) \
    /*CPPUNIT_TEST( Treiber_HP_yield  )*/ \
    CPPUNIT_TEST( Treiber_HP_pause  ) \
    CPPUNIT_TEST( Treiber_HP_exp    ) \
    CPPUNIT_TEST( Treiber_HP_stat   ) \
    CPPUNIT_TEST( Treiber_HRC       ) \
    /*CPPUNIT_TEST( Treiber_HRC_yield )*/ \
    /*CPPUNIT_TEST( Treiber_HRC_pause )*/ \
    /*CPPUNIT_TEST( Treiber_HRC_exp   )*/ \
    CPPUNIT_TEST( Treiber_HRC_stat  ) \
    CPPUNIT_TEST( Treiber_PTB       ) \
    /*CPPUNIT_TEST( Treiber_PTB_yield )*/ \
    CPPUNIT_TEST( Treiber_PTB_pause ) \
    CPPUNIT_TEST( Treiber_PTB_exp   ) \
    CPPUNIT_TEST( Treiber_PTB_stat  )

#define CDSUNIT_DECLARE_EliminationStack \
    TEST_ELIMINATION( Elimination_HP, cds::intrusive::single_link::node< cds::gc::HP >        ) \
    TEST_ELIMINATION( Elimination_HP_2ms, cds::intrusive::single_link::node< cds::gc::HP >    ) \
    TEST_ELIMINATION( Elimination_HP_2ms_stat, cds::intrusive::single_link::node< cds::gc::HP >) \
    TEST_ELIMINATION( Elimination_HP_5ms, cds::intrusive::single_link::node< cds::gc::HP >    ) \
    TEST_ELIMINATION( Elimination_HP_5ms_stat, cds::intrusive::single_link::node< cds::gc::HP >) \
    TEST_ELIMINATION( Elimination_HP_10ms, cds::intrusive::single_link::node< cds::gc::HP >    ) \
    TEST_ELIMINATION( Elimination_HP_10ms_stat, cds::intrusive::single_link::node< cds::gc::HP >) \
    TEST_ELIMINATION( Elimination_HP_seqcst, cds::intrusive::single_link::node< cds::gc::HP > ) \
    /*TEST_ELIMINATION( Elimination_HP_yield, cds::intrusive::single_link::node< cds::gc::HP >  )*/ \
    TEST_ELIMINATION( Elimination_HP_pause, cds::intrusive::single_link::node< cds::gc::HP >  ) \
    TEST_ELIMINATION( Elimination_HP_exp, cds::intrusive::single_link::node< cds::gc::HP >    ) \
    TEST_ELIMINATION( Elimination_HP_stat, cds::intrusive::single_link::node< cds::gc::HP >   ) \
    TEST_ELIMINATION( Elimination_HP_dyn, cds::intrusive::single_link::node< cds::gc::HP >    ) \
    TEST_ELIMINATION( Elimination_HP_dyn_stat, cds::intrusive::single_link::node< cds::gc::HP >) \
    TEST_ELIMINATION( Elimination_HRC, cds::intrusive::single_link::node< cds::gc::HRC >       ) \
    /*TEST_ELIMINATION( Elimination_HRC_yield, cds::intrusive::single_link::node< cds::gc::HRC > )*/ \
    TEST_ELIMINATION( Elimination_HRC_pause, cds::intrusive::single_link::node< cds::gc::HRC > ) \
    TEST_ELIMINATION( Elimination_HRC_exp, cds::intrusive::single_link::node< cds::gc::HRC >   ) \
    TEST_ELIMINATION( Elimination_HRC_stat, cds::intrusive::single_link::node< cds::gc::HRC >  ) \
    TEST_ELIMINATION( Elimination_HRC_dyn, cds::intrusive::single_link::node< cds::gc::HRC >   ) \
    TEST_ELIMINATION( Elimination_HRC_dyn_stat, cds::intrusive::single_link::node< cds::gc::HRC >) \
    TEST_ELIMINATION( Elimination_PTB, cds::intrusive::single_link::node< cds::gc::PTB >       ) \
    TEST_ELIMINATION( Elimination_PTB_2ms, cds::intrusive::single_link::node< cds::gc::PTB >    ) \
    TEST_ELIMINATION( Elimination_PTB_2ms_stat, cds::intrusive::single_link::node< cds::gc::PTB >) \
    TEST_ELIMINATION( Elimination_PTB_5ms, cds::intrusive::single_link::node< cds::gc::PTB >    ) \
    TEST_ELIMINATION( Elimination_PTB_5ms_stat, cds::intrusive::single_link::node< cds::gc::PTB >) \
    TEST_ELIMINATION( Elimination_PTB_10ms, cds::intrusive::single_link::node< cds::gc::PTB >    ) \
    TEST_ELIMINATION( Elimination_PTB_10ms_stat, cds::intrusive::single_link::node< cds::gc::PTB >) \
    /*TEST_ELIMINATION( Elimination_PTB_yield, cds::intrusive::single_link::node< cds::gc::PTB > )*/ \
    TEST_ELIMINATION( Elimination_PTB_pause, cds::intrusive::single_link::node< cds::gc::PTB > ) \
    TEST_ELIMINATION( Elimination_PTB_exp, cds::intrusive::single_link::node< cds::gc::PTB >   ) \
    TEST_ELIMINATION( Elimination_PTB_stat, cds::intrusive::single_link::node< cds::gc::PTB >  ) \
    TEST_ELIMINATION( Elimination_PTB_dyn, cds::intrusive::single_link::node< cds::gc::PTB >   ) \
    TEST_ELIMINATION( Elimination_PTB_dyn_stat, cds::intrusive::single_link::node< cds::gc::PTB >)

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
    CPPUNIT_TEST( Elimination_HRC       ) \
    /*CPPUNIT_TEST( Elimination_HRC_yield )*/ \
    /*CPPUNIT_TEST( Elimination_HRC_pause )*/ \
    /*CPPUNIT_TEST( Elimination_HRC_exp   )*/ \
    CPPUNIT_TEST( Elimination_HRC_stat  ) \
    CPPUNIT_TEST( Elimination_HRC_dyn   ) \
    CPPUNIT_TEST( Elimination_HRC_dyn_stat) \
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
    TEST_BOUNDED( MichaelDequeL_HP, cds::intrusive::michael_deque::node<cds::gc::HP>          ) \
    TEST_BOUNDED( MichaelDequeL_HP_seqcst, cds::intrusive::michael_deque::node<cds::gc::HP>   ) \
    TEST_BOUNDED( MichaelDequeL_HP_ic, cds::intrusive::michael_deque::node<cds::gc::HP>       ) \
    TEST_BOUNDED( MichaelDequeL_HP_exp, cds::intrusive::michael_deque::node<cds::gc::HP>      ) \
    TEST_BOUNDED( MichaelDequeL_HP_yield, cds::intrusive::michael_deque::node<cds::gc::HP>    ) \
    TEST_BOUNDED( MichaelDequeL_HP_stat, cds::intrusive::michael_deque::node<cds::gc::HP>     ) \
    TEST_BOUNDED( MichaelDequeL_PTB, cds::intrusive::michael_deque::node<cds::gc::PTB>         ) \
    TEST_BOUNDED( MichaelDequeL_PTB_seqcst, cds::intrusive::michael_deque::node<cds::gc::PTB>  ) \
    TEST_BOUNDED( MichaelDequeL_PTB_ic, cds::intrusive::michael_deque::node<cds::gc::PTB>      ) \
    TEST_BOUNDED( MichaelDequeL_PTB_exp, cds::intrusive::michael_deque::node<cds::gc::PTB>     ) \
    TEST_BOUNDED( MichaelDequeL_PTB_yield, cds::intrusive::michael_deque::node<cds::gc::PTB>   ) \
    TEST_BOUNDED( MichaelDequeL_PTB_stat, cds::intrusive::michael_deque::node<cds::gc::PTB>    ) \
    TEST_BOUNDED( MichaelDequeR_HP, cds::intrusive::michael_deque::node<cds::gc::HP>          ) \
    TEST_BOUNDED( MichaelDequeR_HP_seqcst, cds::intrusive::michael_deque::node<cds::gc::HP>   ) \
    TEST_BOUNDED( MichaelDequeR_HP_ic, cds::intrusive::michael_deque::node<cds::gc::HP>       ) \
    TEST_BOUNDED( MichaelDequeR_HP_exp, cds::intrusive::michael_deque::node<cds::gc::HP>      ) \
    TEST_BOUNDED( MichaelDequeR_HP_yield, cds::intrusive::michael_deque::node<cds::gc::HP>    ) \
    TEST_BOUNDED( MichaelDequeR_HP_stat, cds::intrusive::michael_deque::node<cds::gc::HP>     ) \
    TEST_BOUNDED( MichaelDequeR_PTB, cds::intrusive::michael_deque::node<cds::gc::PTB>         ) \
    TEST_BOUNDED( MichaelDequeR_PTB_seqcst, cds::intrusive::michael_deque::node<cds::gc::PTB>  ) \
    TEST_BOUNDED( MichaelDequeR_PTB_ic, cds::intrusive::michael_deque::node<cds::gc::PTB>      ) \
    TEST_BOUNDED( MichaelDequeR_PTB_exp, cds::intrusive::michael_deque::node<cds::gc::PTB>     ) \
    TEST_BOUNDED( MichaelDequeR_PTB_yield, cds::intrusive::michael_deque::node<cds::gc::PTB>   ) \
    TEST_BOUNDED( MichaelDequeR_PTB_stat, cds::intrusive::michael_deque::node<cds::gc::PTB>    )

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

#endif // #ifndef __CDSUNIT_INTRUSIVE_STACK_DEFS_H
