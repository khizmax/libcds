CDS C++ library
===============
[![GitHub version](https://badge.fury.io/gh/khizmax%2Flibcds.svg)](http://badge.fury.io/gh/khizmax%2Flibcds)
[![License](https://img.shields.io/:license-bsd-blue.svg?style=round-square)](https://github.com/khizmax/libcds/blob/master/LICENSE)

<!---
The build time for lib and hdr-test is exceed the limit of 50 minutes
[![Build Status](https://travis-ci.org/khizmax/libcds.svg?branch=dev)](https://travis-ci.org/khizmax/libcds)
-->
<!---
The coverity dataset is about 4G of size and about 1G in compressed state so it is a problem to upload it to the coverity server
[![Coverity Scan Build Status](https://scan.coverity.com/projects/4445/badge.svg)](https://scan.coverity.com/projects/4445)
-->

The Concurrent Data Structures (CDS) library is a collection of concurrent containers
that don't require external (manual) synchronization for shared access, and safe memory reclamation (SMR) 
algorithms like [Hazard Pointer](http://en.wikipedia.org/wiki/Hazard_pointer) 
and user-space [RCU](http://en.wikipedia.org/wiki/Read-copy-update) that is used as an epoch-based SMR.

CDS is mostly header-only template library. Only SMR core implementation is segregated to .so/.dll file.

The library contains the implementations of the following containers:
  - [lock-free](http://en.wikipedia.org/wiki/Non-blocking_algorithm) stack with optional elimination support
  - several algo for lock-free queue, including classic Michael & Scott algorithm and its derivatives,
    the flat combining queue, the segmented queue.
  - several implementation of unordered set/map - lock-free and fine-grained lock-based
  - [flat-combining] (http://mcg.cs.tau.ac.il/projects/projects/flat-combining) technique
  - lock-free [skip-list](http://en.wikipedia.org/wiki/Skip_list)
  - lock-free FeldmanHashMap/Set [Multi-Level Array Hash](http://samos-conference.com/Resources_Samos_Websites/Proceedings_Repository_SAMOS/2013/Files/2013-IC-20.pdf)
    with thread-safe bidirectional iterator support
  - Bronson's et al algorithm for fine-grained lock-based AVL tree
  
Generally, each container has an intrusive and non-intrusive (STL-like) version belonging to 
*cds::intrusive* and *cds::container* namespace respectively. 

Version 2.x of the library is written on C++11 and can be compiled by GCC 4.8+, clang 3.6+, Intel C++ 15+, 
and MS VC++ 12 (2013) Update 4 and above

Download the latest release from http://sourceforge.net/projects/libcds/files/

See online doxygen-generated doc here: http://libcds.sourceforge.net/doc/cds-api/index.html

Evolution of libcds (Gource visualization by Landon Wilkins): https://www.youtube.com/watch?v=FHaJvVdmJ0w

**How to build**
   - *nix: [use CMake](build/cmake/readme.md)
   - Windows: use MS Visual C++ 2015 project

**Pull request requirements**
- Pull-request to *master* branch will be unconditionally rejected
- *integration* branch is intended for pull-request. Usually, *integration* branch is the same as *master*
- *dev* branch is intended for main developing. Usually, it contains unstable code

[![Project stats](https://www.openhub.net/p/khizmax-libcds/widgets/project_thin_badge.gif)](https://www.openhub.net/p/khizmax-libcds)

References
----------
*Stack*
  - *TreiberStack*: [1986] R. K. Treiber. Systems programming: Coping with parallelism. Technical Report RJ 5118, IBM Almaden Research Center, April 1986.
  - Elimination back-off implementation is based on idea from [2004] Danny Hendler, Nir Shavit, Lena Yerushalmi "A Scalable Lock-free Stack Algorithm"
        [pdf](http://people.csail.mit.edu/shanir/publications/Lock_Free.pdf)
  - *FCStack* - flat-combining wrapper for *std::stack*
        
*Queue*
  - *BasketQueue*: [2007] Moshe Hoffman, Ori Shalev, Nir Shavit "The Baskets Queue"
        [pdf](http://people.csail.mit.edu/shanir/publications/Baskets%20Queue.pdf)
  - *MSQueue*:
    * [1998] Maged Michael, Michael Scott "Simple, fast, and practical non-blocking and blocking concurrent queue algorithms"
        [pdf](http://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf)
    * [2002] Maged M.Michael "Safe memory reclamation for dynamic lock-free objects using atomic reads and writes"
        [pdf](http://www.research.ibm.com/people/m/michael/podc-2002.pdf)
    * [2003] Maged M.Michael "Hazard Pointers: Safe memory reclamation for lock-free objects"
        [pdf](http://www.research.ibm.com/people/m/michael/ieeetpds-2004.pdf)
  - *RWQueue*: [1998] Maged Michael, Michael Scott "Simple, fast, and practical non-blocking and blocking concurrent queue algorithms"
        [pdf](http://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf)
  - *MoirQueue*: [2000] Simon Doherty, Lindsay Groves, Victor Luchangco, Mark Moir "Formal Verification of a practical lock-free queue algorithm"
        [pdf](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.87.9954&rep=rep1&type=pdf)
  - *OptimisticQueue*: [2008] Edya Ladan-Mozes, Nir Shavit "An Optimistic Approach to Lock-Free FIFO Queues"
        [pdf](https://people.csail.mit.edu/edya/publications/OptimisticFIFOQueue-journal.pdf)
  - *SegmentedQueue*: [2010] Afek, Korland, Yanovsky "Quasi-Linearizability: relaxed consistency for improved concurrency"
        [pdf](http://mcg.cs.tau.ac.il/papers/opodis2010-quasi.pdf)
  - *FCQueue* - flat-combining wrapper for *std::queue*
  - *VyukovMPMCCycleQueue* Dmitry Vyukov (see http://www.1024cores.net)

*Deque*
  - flat-combining deque based on *stl::deque*

*Map, set*
  - *MichaelHashMap*: [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"
        [pdf](http://www.research.ibm.com/people/m/michael/spaa-2002.pdf)
  - *SplitOrderedList*: [2003] Ori Shalev, Nir Shavit "Split-Ordered Lists - Lock-free Resizable Hash Tables"
        [pdf](http://people.csail.mit.edu/shanir/publications/Split-Ordered_Lists.pdf)
  - *StripedMap*, *StripedSet*: [2008] Maurice Herlihy, Nir Shavit "The Art of Multiprocessor Programming"
  - *CuckooMap*, *CuckooSet*: [2008] Maurice Herlihy, Nir Shavit "The Art of Multiprocessor Programming"
  - *SkipListMap*, *SkipListSet*: [2008] Maurice Herlihy, Nir Shavit "The Art of Multiprocessor Programming"
  - *FeldmanHashMap*, *FeldmanHashSet*: [2013] Steven Feldman, Pierre LaBorde, Damian Dechev "Concurrent Multi-level Arrays:
        Wait-free Extensible Hash Maps". Supports **thread-safe bidirectional iterators**
        [pdf](http://samos-conference.com/Resources_Samos_Websites/Proceedings_Repository_SAMOS/2013/Files/2013-IC-20.pdf)
        
*Ordered single-linked list*
  - *LazyList*: [2005] Steve Heller, Maurice Herlihy, Victor Luchangco, Mark Moir, William N. Scherer III, and Nir Shavit "A Lazy Concurrent List-Based Set Algorithm"
        [pdf](http://people.csail.mit.edu/shanir/publications/Lazy_Concurrent.pdf)
  - *MichaelList*: [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"
        [pdf](http://www.research.ibm.com/people/m/michael/spaa-2002.pdf)

*Priority queue*
  - *MSPriorityQueue*: [1996] G.Hunt, M.Michael, S. Parthasarathy, M.Scott "An efficient algorithm for concurrent priority queue heaps"
        [pdf](http://web.cse.ohio-state.edu/dmrl/papers/heap96.pdf)

*Tree*
  - *EllenBinTree*: [2010] F.Ellen, P.Fatourou, E.Ruppert, F.van Breugel "Non-blocking Binary Search Tree"
        [pdf](http://www.cs.vu.nl/~tcs/cm/faith.pdf)
  - *BronsonAVLTreeMap* - lock-based fine-grained AVL-tree implementation: 
        [2010] Nathan Bronson, Jared Casper, Hassan Chafi, Kunle Olukotun "A Practical Concurrent Binary Search Tree"
        [pdf](https://ppl.stanford.edu/papers/ppopp207-bronson.pdf)

*SMR*
  - Hazard Pointers
    * [2002] Maged M.Michael "Safe memory reclamation for dynamic lock-free objects using atomic reads and writes" 
             [pdf](http://www.research.ibm.com/people/m/michael/podc-2002.pdf)
    * [2003] Maged M.Michael "Hazard Pointers: Safe memory reclamation for lock-free objects" 
             [pdf](http://www.research.ibm.com/people/m/michael/ieeetpds-2004.pdf)
    * [2004] Andrei Alexandrescy, Maged Michael "Lock-free Data Structures with Hazard Pointers" 
             [pdf](http://www.researchgate.net/profile/Andrei_Alexandrescu/publication/252573326_Lock-Free_Data_Structures_with_Hazard_Pointers/links/0deec529e7804288fe000000.pdf)
  - User-space RCU
    * [2009] M.Desnoyers "Low-Impact Operating System Tracing" PhD Thesis,
             Chapter 6 "User-Level Implementations of Read-Copy Update"
             [pdf](http://www.lttng.org/files/thesis/desnoyers-dissertation-2009-12-v27.pdf)
    * [2011] M.Desnoyers, P.McKenney, A.Stern, M.Dagenias, J.Walpole "User-Level
             Implementations of Read-Copy Update"
             [pdf](http://www.dorsal.polymtl.ca/sites/www.dorsal.polymtl.ca/files/publications/desnoyers-ieee-urcu-submitted.pdf)

*Flat Combining* technique
  - [2010] Hendler, Incze, Shavit and Tzafrir "Flat Combining and the Synchronization-Parallelism Tradeoff"
            [pdf](http://www.cs.bgu.ac.il/~hendlerd/papers/flat-combining.pdf)
