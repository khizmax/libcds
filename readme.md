CDS C++ library
===============

The Concurrent Data Structures (CDS) library is a collection of concurrent containers
that don't require external (manual) synchronization for shared access, and safe memory reclamation (SMR) 
algorithms like [Hazard Pointer](http://en.wikipedia.org/wiki/Hazard_pointer) 
and user-space [RCU](http://en.wikipedia.org/wiki/Read-copy-update). 
CDS is mostly header-only template library. Only SMR core implementation is segregated to .so/.dll file.

The library contains the implementations of the following containers:
  - [lock-free](http://en.wikipedia.org/wiki/Non-blocking_algorithm) stack with optional elimination support
  - several algo for lock-free queue, including classic Michael & Scott algorithm and its derivatives,
    the flat combining queue, the segmented queue.
  - several implementation of unordered set/map - lock-free and fine-grained lock-based
  - [flat-combining] (http://mcg.cs.tau.ac.il/projects/projects/flat-combining) technique
  - lock-free [skip-list](http://en.wikipedia.org/wiki/Skip_list)
  
Generally, each container has an intrusive and non-intrusive (STL-like) version belonging to 
*cds::intrusive* and *cds::container* namespace respectively.

Version 2.x of the library is written on C++11 and can be compiled by GCC 4.8+, clang 3.3+, Intel C++ 15+, 
and MS VC++ 12 (2013) Update 4.

Download the latest release from http://sourceforge.net/projects/libcds/files/

See online doxygen-generated doc here: http://libcds.sourceforge.net/doc/cds-api/index.html

References
----------
*Stack*
  - *TreiberStack*: [1986] R. K. Treiber. Systems programming: Coping with parallelism. Technical Report RJ 5118, IBM Almaden Research Center, April 1986.
  - Elimination back-off implementation is based on idea from [2004] Danny Hendler, Nir Shavit, Lena Yerushalmi "A Scalable Lock-free Stack Algorithm"
  - *FCStack* - flat-combining wrapper for *std::stack*
        
*Queue*
  - *BasketQueue*: [2007] Moshe Hoffman, Ori Shalev, Nir Shavit "The Baskets Queue"
  - *MSQueue*:
    * [1998] Maged Michael, Michael Scott "Simple, fast, and practical non-blocking and blocking concurrent queue algorithms"
    * [2002] Maged M.Michael "Safe memory reclamation for dynamic lock-free objects using atomic reads and writes"
    * [2003] Maged M.Michael "Hazard Pointers: Safe memory reclamation for lock-free objects"
  - *RWQueue*: [1998] Maged Michael, Michael Scott "Simple, fast, and practical non-blocking and blocking concurrent queue algorithms"
  - *MoirQueue*: [2000] Simon Doherty, Lindsay Groves, Victor Luchangco, Mark Moir "Formal Verification of a practical lock-free queue algorithm"
  - *OptimisticQueue*: [2008] Edya Ladan-Mozes, Nir Shavit "An Optimistic Approach to Lock-Free FIFO Queues"
  - *SegmentedQueue*: [2010] Afek, Korland, Yanovsky "Quasi-Linearizability: relaxed consistency for improved concurrency"
  - *FCQueue* - flat-combining wrapper for *std::queue*
  - *TsigasCycleQueue*: [2000] Philippas Tsigas, Yi Zhang "A Simple, Fast and Scalable Non-Blocking Concurrent FIFO Queue for Shared Memory Multiprocessor Systems"
  - *VyukovMPMCCycleQueue* Dmitry Vyukov (see http://www.1024cores.net)

*Deque*
  - flat-combining deque based on *stl::deque*

*Map, set*
  - *MichaelHashMap*: [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"
  - *SplitOrderedList*: [2003] Ori Shalev, Nir Shavit "Split-Ordered Lists - Lock-free Resizable Hash Tables"
  - *StripedMap*, *StripedSet*: [2008] Maurice Herlihy, Nir Shavit "The Art of Multiprocessor Programming"
  - *CuckooMap*, *CuckooSet*: [2008] Maurice Herlihy, Nir Shavit "The Art of Multiprocessor Programming"
  - *SkipListMap*, *SkipListSet*: [2008] Maurice Herlihy, Nir Shavit "The Art of Multiprocessor Programming"
        
*Ordered single-linked list*
  - *LazyList*: [2005] Steve Heller, Maurice Herlihy, Victor Luchangco, Mark Moir, William N. Scherer III, and Nir Shavit "A Lazy Concurrent List-Based Set Algorithm"
  - *MichaelList*: [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"

*Priority queue*
  - *MSPriorityQueue*: [1996] G.Hunt, M.Michael, S. Parthasarathy, M.Scott "An efficient algorithm for concurrent priority queue heaps"

*Tree*
  - *EllenBinTree*: [2010] F.Ellen, P.Fatourou, E.Ruppert, F.van Breugel "Non-blocking Binary Search Tree"

*SMR*
  - Hazard Pointers
    * [2002] Maged M.Michael "Safe memory reclamation for dynamic lock-freeobjects using atomic reads and writes"
    * [2003] Maged M.Michael "Hazard Pointers: Safe memory reclamation for lock-free objects"
    * [2004] Andrei Alexandrescy, Maged Michael "Lock-free Data Structures with Hazard Pointers"
  - User-space RCU
    * [2009] M.Desnoyers "Low-Impact Operating System Tracing" PhD Thesis,
             Chapter 6 "User-Level Implementations of Read-Copy Update"
    * [2011] M.Desnoyers, P.McKenney, A.Stern, M.Dagenias, J.Walpole "User-Level
             Implementations of Read-Copy Update"

*Memory allocation*
  - [2004] M.Michael "Scalable Lock-free Dynamic Memory Allocation"

*Flat Combining* technique
  - [2010] Hendler, Incze, Shavit and Tzafrir "Flat Combining and the Synchronization-Parallelism Tradeoff"
