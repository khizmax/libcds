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

#ifndef CDSLIB_INTRUSIVE_MICHAEL_DEQUE_H
#define CDSLIB_INTRUSIVE_MICHAEL_DEQUE_H

#include <type_traits>
#include <cds/algo/atomic.h>
#include <cds/intrusive/details/double_link_struct.h>

namespace cds
{
namespace intrusive
{

/// michael_deque related definitions
/** @ingroup cds_intrusive_helper
*/
namespace michael_deque
{

/// Deque node
/**
    Template parameters:
    - GC - garbage collector used
    - Tag - a \ref cds_intrusive_hook_tag "tag"
*/
template <class GC, typename Tag = opt::none >
using node = cds::intrusive::double_link::node< GC, Tag >;

/// Base hook
/**
    \p Options are:
    - opt::gc - garbage collector used.
    - opt::tag - a \ref cds_intrusive_hook_tag "tag"
*/
template < typename... Options >
using base_hook = cds::intrusive::double_link::base_hook< Options...>;

/// Member hook
/**
    \p MemberOffset specifies offset in bytes of \ref node member into your structure.
    Use \p offsetof macro to define \p MemberOffset

    \p Options are:
    - opt::gc - garbage collector used.
    - opt::tag - a \ref cds_intrusive_hook_tag "tag"
*/
template < size_t MemberOffset, typename... Options >
using member_hook = cds::intrusive::double_link::member_hook< MemberOffset, Options... >;

/// Traits hook
/**
    \p NodeTraits defines type traits for node.
    See \ref node_traits for \p NodeTraits interface description

    \p Options are:
    - opt::gc - garbage collector used.
    - opt::tag - a \ref cds_intrusive_hook_tag "tag"
*/
template <typename NodeTraits, typename... Options >
using traits_hook = cds::intrusive::double_link::traits_hook< NodeTraits, Options... >;

/// Deque internal statistics. May be used for debugging or profiling
/**
    Template argument \p Counter defines type of counter.
    Default is \p cds::atomicity::event_counter, that is weak, i.e. it is not guaranteed
    strict event counting.
    You may use stronger type of counter like as \p cds::atomicity::item_counter,
    or even integral type, for example, \p int.
*/
template <typename Counter = cds::atomicity::event_counter >
struct stat
{
  typedef Counter     counter_type;   ///< Counter type

  counter_type    m_nPushLeft      ;  ///< Count of push_left operations
  counter_type    m_nPushRight     ;  ///< Count of push_right operations
  counter_type    m_nPopLeft       ;  ///< Count of success pop_left operations
  counter_type    m_nPopRight      ;  ///< Count of success pop_right operations
  counter_type    m_nFailedPopLeft ;  ///< Count of failed pop_left operations (pop from empty deque)
  counter_type    m_nFailedPopRight;  ///< Count of failed pop_right operations (pop from empty deque)
  counter_type    m_nStabilizeRight;  ///< How many tries to stabilize deque
  counter_type    m_nStabilizeLeft ;  ///< How many tries to stabilize deque

  //@cond
  void    onPushLeft()               { ++m_nPushLeft; }
  void    onPushRight()              { ++m_nPushRight; }
  void    onPopLeft(bool succ = true)    { if (succ) ++m_nPopLeft; else ++m_nFailedPopLeft;  }
  void    onPopRight(bool succ = true)   { if (succ) ++m_nPopRight; else ++m_nFailedPopRight;  }
  void    onStabilizeRight()         { ++m_nStabilizeRight; }
  void    onStabilizeLeft()          { ++m_nStabilizeLeft; }

  void reset()
  {
    m_nPushLeft.reset();
    m_nPushRight.reset();
    m_nPopLeft.reset();
    m_nPopRight.reset();
    m_nFailedPopLeft.reset();
    m_nFailedPopRight.reset();
    m_nStabilizeRight.reset();
    m_nStabilizeLeft.reset();
  }

  stat& operator +=(stat const& s)
  {
    m_nPushLeft += s.m_nPushLeft.get();
    m_nPushRight += s.m_nPushRight.get();
    m_nPopLeft += s.m_nPopLeft.get();
    m_nPopRight += s.m_nPopRight.get();
    m_nFailedPopLeft += s.m_nFailedPopLeft.get();
    m_nFailedPopRight += s.m_nFailedPopRight.get();
    m_nStabilizeRight += s.m_nStabilizeRight.get();
    m_nStabilizeLeft += s.m_nStabilizeLeft.get();

    return *this;
  }
  //@endcond
};

/// Dummy deque statistics - no counting is performed, no overhead. Support interface like \p michael_deque::stat
struct empty_stat
{
  //@cond
  void    onPushLeft()               const { }
  void    onPushRight()              const { }
  void    onPopLeft(bool succ = true)    const { (void)(succ); } //to hide warning
  void    onPopRight(bool succ = true)   const { (void)(succ); }
  void    onStabilizeRight()         const { }
  void    onStabilizeLeft()          const { }

  void reset() {}
  empty_stat& operator +=(empty_stat const&)
  {
    return *this;
  }
  //@endcond
};

/// MichaelDeque default traits
struct traits
{
  /// Hook, possible types are \p michael_deque::base_hook, \p michael_deque::member_hook, \p michael_deque::traits_hook
  typedef michael_deque::base_hook<>        hook;

  /// The functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used for dequeuing
  typedef opt::v::empty_disposer      disposer;

  /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
  typedef atomicity::empty_item_counter   item_counter;

  /// Internal statistics (by default, disabled)
  /**
      Possible option value are: \p michael_deque::stat, \p michael_deque::empty_stat (the default),
      user-provided class that supports \p %michael_deque::stat interface.
  */
  typedef michael_deque::empty_stat         stat;

  /// C++ memory ordering model
  /**
      Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
      or \p opt::v::sequential_consistent (sequentially consisnent memory model).
  */
  typedef opt::v::relaxed_ordering    memory_model;

  /// Link checking, see \p cds::opt::link_checker
  static CDS_CONSTEXPR const opt::link_check_type link_checker = opt::debug_check_link;

  /// Padding for internal critical atomic data. Default is \p opt::cache_line_padding
  enum { padding = opt::cache_line_padding };
};

/// Metafunction converting option list to \p michael_deque::traits
/**
    Supported \p Options are:

    - \p opt::hook - hook used. Possible hooks are: \p michael_deque::base_hook, \p michael_deque::member_hook, \p michael_deque::traits_hook.
        If the option is not specified, \p %michael_deque::base_hook<> is used.
    - \p opt::disposer - the functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used
        when dequeuing.
    - \p opt::link_checker - the type of node's link fields checking. Default is \p opt::debug_check_link
    - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
        To enable item counting use \p cds::atomicity::item_counter
    - \p opt::stat - the type to gather internal statistics.
        Possible statistics types are: \p michael_deque::stat, \p michael_deque::empty_stat, user-provided class that supports \p %michael_deque::stat interface.
        Default is \p %michael_deque::empty_stat (internal statistics disabled).
    - \p opt::padding - padding for internal critical atomic data. Default is \p opt::cache_line_padding
    - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
        or \p opt::v::sequential_consistent (sequentially consisnent memory model).

    Example: declare \p %MichaelDeque with item counting and internal statistics
    \code
    typedef cds::intrusive::MichaelDeque< cds::gc::HP, Foo,
        typename cds::intrusive::michael_deque::make_traits<
            cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc<cds::gc::HP> >>,
            cds::opt::item_counter< cds::atomicity::item_counter >,
            cds::opt::stat< cds::intrusive::michael_deque::stat<> >
        >::type
    > myDeque;
    \endcode
*/

template <typename... Options>
struct make_traits
{
#   ifdef CDS_DOXYGEN_INVOKED
  typedef implementation_defined type;   ///< Metafunction result
#   else
  typedef typename cds::opt::make_options <
  typename cds::opt::find_type_traits< traits, Options... >::type
  , Options...
  >::type type;
#   endif
};
} // namespace michael_deque

/// CAS-Based Lock-Free Algorithm for Shared Deques
/** @ingroup cds_intrusive_deque
    Implementation of CAS-Based Lock-Free Algorithm for Shared Deques:
    - [2003] Maged M. Michael "CAS-Based Lock-Free Algorithm for Shared Deques"

    Template arguments:
    - \p GC - garbage collector type: \p gc::HP, \p gc::DHP
    - \p T - type of value to be stored in the deque. A value of type \p T must be derived from \p michael_deque::node for \p michael_deque::base_hook,
        or it should have a member of type \p %michael_deque::node for \p michael_deque::member_hook,
        or it should be convertible to \p %michael_deque::node for \p michael_deque::traits_hook.
    - \p Traits - deque traits, default is \p michael_deque::traits. You can use \p michael_deque::make_traits
        metafunction to make your traits or just derive your traits from \p %michael_deque::traits:
        \code
        struct myTraits: public cds::intrusive::michael_deque::traits {
            typedef cds::intrusive::michael_deque::stat<> stat;
            typedef cds::atomicity::item_counter    item_counter;
        };
        typedef cds::intrusive::MichaelDeque< cds::gc::HP, Foo, myTraits > myDeque;

        // Equivalent make_traits example:
        typedef cds::intrusive::MichaelDeque< cds::gc::HP, Foo,
            typename cds::intrusive::michael_deque::make_traits<
                cds::opt::stat< cds::intrusive::michael_deque::stat<> >,
                cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > myDeque;
        \endcode

    \par Examples
    \code
    #include <cds/intrusive/michael_deque.h>
    #include <cds/gc/hp.h>

    namespace ci = cds::intrusive;
    typedef cds::gc::HP hp_gc;

    // MichaelDeque with Hazard Pointer garbage collector, base hook + item disposer:
    struct Foo: public ci::michael_deque::node< hp_gc >
    {
        // Your data
        ...
    };

    // Disposer for Foo struct just deletes the object passed in
    struct fooDisposer {
        void operator()( Foo * p )
        {
            delete p;
        }
    };

    // Declare traits for the deque
    struct myTraits: public ci::michael_deque::traits {
        typedef ci::opt::hook<ci::michael_deque::base_hook< ci::opt::gc<hp_gc> >> hook;
        typedef ci::opt::disposer< fooDisposer > disposer;
    };

    // At least, declare the deque type
    typedef ci::MichaelDeque< hp_gc, Foo, myTraits > fooDeque;

    // Example 2:
    //  MichaelDeque with Hazard Pointer garbage collector,
    //  member hook + item disposer + item counter,
    //  without padding of internal deque data
    //  Use michael_deque::make_traits
    struct Bar
    {
        // Your data
        ...
        ci::michael_deque::node< hp_gc > hMember;
    };

    typedef ci::MichaelDeque< hp_gc,
        Foo,
        typename ci::michael_deque::make_traits<
            ci::opt::hook<
                ci::michael_deque::member_hook<
                    offsetof(Bar, hMember)
                    ,ci::opt::gc<hp_gc>
                >
            >
            ,ci::opt::disposer< fooDisposer >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
            ,cds::opt::padding< cds::opt::no_special_padding >
        >::type
    > barDeque;
    \endcode
*/
template <typename GC, typename T, typename Traits = michael_deque::traits>
class MichaelDeque
{
public:
  typedef GC gc;          ///< Garbage collector
  typedef T  value_type;  ///< type of value to be stored in the deque
  typedef Traits traits;  ///< Deque traits

  typedef typename traits::hook       hook;       ///< hook type
  typedef typename hook::node_type    node_type;  ///< node type
  typedef typename traits::disposer   disposer;   ///< disposer used
  typedef typename get_node_traits< value_type, node_type, hook>::type node_traits;   ///< node traits
  typedef typename double_link::get_link_checker< node_type, traits::link_checker >::type link_checker;   ///< link checker

  typedef typename traits::item_counter item_counter; ///< Item counter class
  typedef typename traits::stat       stat;           ///< Internal statistics
  typedef typename traits::memory_model memory_model; ///< Memory ordering. See \p cds::opt::memory_model option

  /// Rebind template arguments
  template <typename GC2, typename T2, typename Traits2>
  struct rebind
  {
    typedef MichaelDeque< GC2, T2, Traits2 > other;   ///< Rebinding result
  };

  static CDS_CONSTEXPR const size_t c_nHazardPtrCount = 3; ///< Count of hazard pointer required for the algorithm

protected:
  //@cond

  // GC and node_type::gc must be the same
  static_assert((std::is_same<gc, typename node_type::gc>::value), "GC and node_type::gc must be the same");

  typedef typename node_type::atomic_node_ptr atomic_node_ptr;

  /// Status flag for anchor
  enum STATUS
  {
    STABLE,
    LPUSH,
    RPUSH
  };

  /// Deque's anchor stores two marked pointers to nodes; first bit of every pointer defines status flag
  struct anchor_type
  {
  protected:
    cds::details::marked_ptr<node_type, 1> m_pLeft; /// < Left pointer
    cds::details::marked_ptr<node_type, 1> m_pRight; /// < Right pointer

  public:
    /// Not equal operator to compare anchors
    friend bool operator !=(anchor_type anchor1, anchor_type anchor2)
    {
      return anchor1.m_pLeft != anchor2.m_pLeft || anchor1.m_pRight != anchor2.m_pRight;
    }

    /// Status getter
    STATUS getStatus()
    {
      //get first bit of two pointers
      bool bit1 = m_pLeft.bits();
      bool bit2 = m_pRight.bits();

      assert(bit1 || bit2);

      if (bit1 && bit2)
        return STATUS::STABLE;
      else if (bit1)
        return STATUS::LPUSH;
      else //if (bit2)
        return STATUS::RPUSH;
    }

    /// Left pointer getter
    node_type* getLeft()
    {
      return m_pLeft.ptr();
    }

    /// Right pointer getter
    node_type* getRight()
    {
      return m_pRight.ptr();
    }

    /// Anchor constructor
    static anchor_type create(node_type* left, node_type* right, STATUS s)
    {
      anchor_type result;
      //set pointers
      result.m_pLeft = cds::details::marked_ptr<node_type, 1>(left);
      result.m_pRight = cds::details::marked_ptr<node_type, 1>(right);

      //set first bit of pointers
      if (s == STATUS::STABLE)
        {
          result.m_pLeft |= 1;
          result.m_pRight |= 1;
        }
      else if (s == STATUS::LPUSH)
        result.m_pLeft |= 1;
      else //if (STATUS::RPUSH)
        result.m_pRight |= 1;

      return result;
    }
  };

  typedef typename gc::template atomic_type<anchor_type> atomic_anchor_type;

  atomic_anchor_type m_Anchor; ///< Deque's anchor
  typename opt::details::apply_padding< atomic_anchor_type, traits::padding >::padding_type pad1_;
  item_counter       m_ItemCounter; ///< Item counter
  stat               m_Stat;        ///< Internal statistics
  //@endcond

  /// Stabilization procedure of deque
  void stabilize_right(anchor_type anchor)
  {
    m_Stat.onStabilizeRight();

    typename gc::Guard guard;
    //protect anchor pointers
    node_type* anchor_left = guard.assign(anchor.getLeft());
    node_type* anchor_right = guard.assign(anchor.getRight());
    //check if anchor hasn't changed
    if (anchor != m_Anchor.load(memory_model::memory_order_acquire))
      return;
    //protect pointer to previous node
    node_type* prev = guard.protect(anchor_right->m_pLeft);
    //check if anchor hasn't changed
    if (anchor != m_Anchor.load(memory_model::memory_order_acquire))
      return;
    //take right pointer of previous node
    node_type* prevnext = prev->m_pRight.load(memory_model::memory_order_acquire);
    //if it isn't equal with right pointer of anchor (if they don't point to our new node)
    if (prevnext != anchor_right)
      {
        //check if anchor hasn't changed
        if (anchor != m_Anchor.load(memory_model::memory_order_acquire))
          return;
        //CAS right pointer of previous node
        if (!prev->m_pRight.compare_exchange_strong(prevnext, anchor_right, memory_model::memory_order_release, atomics::memory_order_relaxed))
          return;
      }
    //CAS anchor status to stable
    anchor_type new_anchor = anchor_type::create(anchor_left, anchor_right, STATUS::STABLE);
    m_Anchor.compare_exchange_strong(anchor, new_anchor, memory_model::memory_order_release, atomics::memory_order_relaxed);
  }

  /// Stabilization procedure of deque
  void stabilize_left(anchor_type anchor)
  {
    m_Stat.onStabilizeLeft();

    typename gc::Guard guard;
    //protect anchor pointers
    node_type* anchor_left = guard.assign(anchor.getLeft());
    node_type* anchor_right = guard.assign(anchor.getRight());
    //check if anchor hasn't changed
    if (anchor != m_Anchor.load(memory_model::memory_order_acquire))
      return;
    //protect pointer to previous node
    node_type* prev = guard.protect(anchor_left->m_pRight);
    //check if anchor hasn't changed
    if (anchor != m_Anchor.load(memory_model::memory_order_acquire))
      return;
    //take left pointer of previous node
    node_type* prevnext = prev->m_pLeft.load(memory_model::memory_order_acquire);
    //if it isn't equal with left pointer of anchor (if they don't point to our new node)
    if (prevnext != anchor_left)
      {
        //check if anchor hasn't changed
        if (anchor != m_Anchor.load(memory_model::memory_order_acquire))
          return;
        //CAS left pointer of previous node
        if (!prev->m_pLeft.compare_exchange_strong(prevnext, anchor_left, memory_model::memory_order_release, atomics::memory_order_relaxed))
          return;
      }
    //CAS anchor status to stable
    anchor_type new_anchor = anchor_type::create(anchor_left, anchor_right, STATUS::STABLE);
    m_Anchor.compare_exchange_strong(anchor, new_anchor, memory_model::memory_order_release, atomics::memory_order_relaxed);
  }

  /// Stabilization procedure of deque
  void stabilize(anchor_type anchor)
  {
    if (anchor.getStatus() == STATUS::RPUSH)
      stabilize_right(anchor);
    else
      stabilize_left(anchor);
  }


  //@cond

  static void clear_links(node_type* pNode)
  {
    pNode->m_pLeft.store(nullptr, memory_model::memory_order_release);
    pNode->m_pRight.store(nullptr, memory_model::memory_order_release);
  }


  void dispose_node(value_type* p)
  {

    struct disposer_thunk
    {
      void operator()(value_type* p) const
      {
        assert(p != nullptr);
        MichaelDeque::clear_links(node_traits::to_node_ptr(p));
        disposer()(p);
      }
    };

    gc::template retire<disposer_thunk>(p);
  }
  //@endcond

public:
  /// Initializes empty deque
  MichaelDeque()
  {
    anchor_type new_anchor = anchor_type::create(nullptr, nullptr, STATUS::STABLE);
    m_Anchor.store(new_anchor, memory_model::memory_order_release);
  }

  /// Destructor clears the deque
  ~MichaelDeque()
  {
    clear();

    anchor_type anchor = m_Anchor.load(memory_model::memory_order_relaxed);
    assert(anchor.getLeft() == nullptr);
    assert(anchor.getRight() == nullptr);
  }

  /// Push element to right side of deque
  bool push_right(value_type& val)
  {
    //create new node
    node_type* new_node_ptr = node_traits::to_node_ptr(val);
    link_checker::is_empty(new_node_ptr);
    while (true)
      {
        //take current anchor
        anchor_type anchor = m_Anchor.load(memory_model::memory_order_acquire);
        //if deque is empty
        if (anchor.getRight() == nullptr)
          {
            //CAS anchor to anchor with pointers to our new element
            anchor_type new_anchor = anchor_type::create(new_node_ptr, new_node_ptr, anchor.getStatus());
            if (m_Anchor.compare_exchange_strong(anchor, new_anchor, memory_model::memory_order_release, atomics::memory_order_relaxed))
              break;
          }
        //if deque isn't empty and stable
        else if (anchor.getStatus() == STATUS::STABLE)
          {
            //set left pointer of new node to rightmost node of deque
            new_node_ptr->m_pLeft.store(anchor.getRight(), memory_model::memory_order_release);
            //CAS anchor: right pointer to new node, status is RPUSH
            anchor_type new_anchor = anchor_type::create(anchor.getLeft(), new_node_ptr, STATUS::RPUSH);
            if (m_Anchor.compare_exchange_strong(anchor, new_anchor, memory_model::memory_order_release, atomics::memory_order_relaxed))
              {
                //try to stabilize deque
                stabilize_right(new_anchor);
                break;
              }
          }
        else
          //try to stabilize deque
          stabilize(anchor);
      }
    ++m_ItemCounter;
    m_Stat.onPushRight();
    return true;
  }

  /// Pop element from right side of deque
  value_type* pop_right()
  {
    typename gc::Guard guard;
    //pointer to poped element
    node_type* anchor_right;
    while (true)
      {
        //take current anchor
        anchor_type anchor = m_Anchor.load(memory_model::memory_order_acquire);
        //if deque if empty
        if (anchor.getRight() == nullptr)
          {
            m_Stat.onPopRight(false);
            return nullptr;
          }
        //if deque stores one element
        else if (anchor.getRight() == anchor.getLeft())
          {
            //protect pointer to poped element
            anchor_right = guard.assign(anchor.getRight());
            //CAS anchor to anchor with nullptr
            anchor_type new_anchor = anchor_type::create(nullptr, nullptr, anchor.getStatus());
            if (m_Anchor.compare_exchange_strong(anchor, new_anchor, memory_model::memory_order_release, atomics::memory_order_relaxed))
              break;
          }
        //if deque stores more then one element
        else if (anchor.getStatus() == STATUS::STABLE)
          {
            //protect anchor pointers
            node_type* anchor_left = guard.assign(anchor.getLeft());
            anchor_right = guard.assign(anchor.getRight());
            //if anchor changed then continue
            if (anchor != m_Anchor.load(memory_model::memory_order_acquire))
              continue;
            //take pointer to previous element from right side
            node_type* prev = anchor_right->m_pLeft.load(memory_model::memory_order_acquire);
            //CAS anchor: right pointer to previous element
            anchor_type new_anchor = anchor_type::create(anchor_left, prev, anchor.getStatus());
            if (m_Anchor.compare_exchange_strong(anchor, new_anchor, memory_model::memory_order_release, atomics::memory_order_relaxed))
              break;
          }
        else
          //try to stabilize deque
          stabilize(anchor);
      }

    m_Stat.onPopRight();
    --m_ItemCounter;

    //get and return value from node
    value_type* result = node_traits::to_value_ptr(anchor_right);
    dispose_node(result);
    return result;
  }


  /// Push element to left side of deque
  bool push_left(value_type& val)
  {
    //create new node
    node_type* new_node_ptr = node_traits::to_node_ptr(val);
    link_checker::is_empty(new_node_ptr);
    while (true)
      {
        //take current anchor
        anchor_type anchor = m_Anchor.load(memory_model::memory_order_acquire);
        //if deque is empty
        if (anchor.getLeft() == nullptr)
          {
            //CAS anchor to anchor with pointers to our new element
            anchor_type new_anchor = anchor_type::create(new_node_ptr, new_node_ptr, anchor.getStatus());
            if (m_Anchor.compare_exchange_strong(anchor, new_anchor, memory_model::memory_order_release, atomics::memory_order_relaxed))
              break;
          }
        //if deque isn't empty and stable
        else if (anchor.getStatus() == STATUS::STABLE)
          {
            //set right pointer of new node to leftmost node of deque
            new_node_ptr->m_pRight.store(anchor.getLeft(), memory_model::memory_order_release);
            //CAS anchor: right pointer to new node, status is LPUSH
            anchor_type new_anchor = anchor_type::create(new_node_ptr, anchor.getRight(), STATUS::LPUSH);
            if (m_Anchor.compare_exchange_strong(anchor, new_anchor, memory_model::memory_order_release, atomics::memory_order_relaxed))
              {
                //try to stabilize deque
                stabilize_left(new_anchor);
                break;
              }
          }
        else
          //try to stabilize deque
          stabilize(anchor);
      }
    ++m_ItemCounter;
    m_Stat.onPushLeft();
    return true;
  }

  /// Pop element from left side of deque
  value_type* pop_left()
  {
    typename gc::Guard guard;
    //pointer to poped element
    node_type* anchor_left;
    while (true)
      {
        //take current anchor
        anchor_type anchor = m_Anchor.load(memory_model::memory_order_acquire);
        //if deque if empty
        if (anchor.getLeft() == nullptr)
          {
            m_Stat.onPopLeft(false);
            return nullptr;
          }
        //if deque stores one element
        else if (anchor.getRight() == anchor.getLeft())
          {
            //protect pointer to poped element
            anchor_left = guard.assign(anchor.getLeft());
            //CAS anchor to anchor with nullptr
            anchor_type new_anchor = anchor_type::create(nullptr, nullptr, anchor.getStatus());
            if (m_Anchor.compare_exchange_strong(anchor, new_anchor, memory_model::memory_order_release, atomics::memory_order_relaxed))
              break;
          }
        //if deque stores more then one element
        else if (anchor.getStatus() == STATUS::STABLE)
          {
            //protect anchor pointers
            node_type* anchor_right = guard.assign(anchor.getRight());
            anchor_left = guard.assign(anchor.getLeft());
            //if anchor changed then continue
            if (anchor != m_Anchor.load(memory_model::memory_order_acquire))
              continue;
            //take pointer to previous element from left side
            node_type* prev = anchor_left->m_pRight.load(memory_model::memory_order_acquire);
            //CAS anchor: left pointer to previous element
            anchor_type new_anchor = anchor_type::create(prev, anchor_right, anchor.getStatus());
            if (m_Anchor.compare_exchange_strong(anchor, new_anchor, memory_model::memory_order_release, atomics::memory_order_relaxed))
              break;
          }
        else
          //try to stabilize deque
          stabilize(anchor);
      }

    m_Stat.onPopLeft();
    --m_ItemCounter;

    //get and return value from node
    value_type* result = node_traits::to_value_ptr(anchor_left);
    dispose_node(result);
    return result;
  }

  /// Checks if the deque is empty
  bool empty() const
  {
    return m_Anchor.load(memory_model::memory_order_relaxed).getRight() == nullptr;
  }

  /// Clear the deque
  /**
      The function repeatedly calls \p deque() until it returns \p nullptr.
      The disposer defined in template \p Traits is called for each item
      that can be safely disposed.
  */
  void clear()
  {
    while (pop_right() != nullptr);
  }

  /// Returns deque's item count
  /**
      The value returned depends on \p michael_deque::traits::item_counter. For \p atomicity::empty_item_counter,
      this function always returns 0.

      @note Even if you use real item counter and it returns 0, this fact is not mean that the deque
      is empty. To check deque emptyness use \p empty() method.
  */
  size_t size() const
  {
    return m_ItemCounter.value();
  }

  /// Returns reference to internal statistics
  stat const& statistics() const
  {
    return m_Stat;
  }
};

}
} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_MICHAEL_DEQUE_H
