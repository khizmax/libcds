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

#include "test_intrusive_deque.h"

#include <cds/gc/dhp.h>
#include <cds/intrusive/michael_deque.h>
#include <vector>

namespace
{
namespace ci = cds::intrusive;

typedef cds::gc::DHP gc_type;
namespace cg = cds::gc::dhp;

class IntrusiveMichaelDeque_DHP : public cds_test::intrusive_michael_deque
{
  typedef cds_test::intrusive_michael_deque base_class;

protected:
  typedef typename base_class::base_hook_item< ci::michael_deque::node<gc_type>> base_item_type;
  typedef typename base_class::member_hook_item< ci::michael_deque::node<gc_type>> member_item_type;

  void SetUp()
  {
    typedef ci::MichaelDeque< gc_type, base_item_type > queue_type;
    cg::GarbageCollector::Construct();
    cds::threading::Manager::attachThread();
  }

  void TearDown()
  {
    cds::threading::Manager::detachThread();
    cg::GarbageCollector::Destruct(true);
  }

  template <typename V>
  void check_array(V& arr)
  {
    for (size_t i = 0; i < arr.size(); ++i)
      {
        ASSERT_EQ(arr[i].nDisposeCount, 2);
      }
  }
};

TEST_F(IntrusiveMichaelDeque_DHP, defaulted)
{
  typedef cds::intrusive::MichaelDeque < gc_type, base_item_type,
          typename ci::michael_deque::make_traits <
          ci::opt::disposer< mock_disposer >
          , ci::opt::hook< ci::michael_deque::base_hook< ci::opt::gc<gc_type>>>
          >::type
          > test_deque;

  std::vector<base_item_type> arr;
  arr.resize(100);
  {
    test_deque q;
    test(q, arr);
  }
  gc_type::scan();
  check_array(arr);
}

TEST_F(IntrusiveMichaelDeque_DHP, base_hook)
{
  typedef cds::intrusive::MichaelDeque< gc_type, base_item_type,
          typename ci::michael_deque::make_traits<
          ci::opt::disposer< mock_disposer >
          , ci::opt::hook< ci::michael_deque::base_hook< ci::opt::gc<gc_type>>>
          >::type
          > test_deque;

  std::vector<base_item_type> arr;
  arr.resize(100);
  {
    test_deque q;
    test(q, arr);
  }
  gc_type::scan();
  check_array(arr);
}

TEST_F(IntrusiveMichaelDeque_DHP, base_item_counting)
{
  typedef cds::intrusive::MichaelDeque< gc_type, base_item_type,
          typename ci::michael_deque::make_traits<
          ci::opt::disposer< mock_disposer >
          , cds::opt::item_counter< cds::atomicity::item_counter >
          , ci::opt::hook< ci::michael_deque::base_hook< ci::opt::gc<gc_type>>>
          >::type
          > test_deque;

  std::vector<base_item_type> arr;
  arr.resize(100);
  {
    test_deque q;
    test(q, arr);
  }
  gc_type::scan();
  check_array(arr);
}

TEST_F(IntrusiveMichaelDeque_DHP, base_stat)
{
  struct traits : public ci::michael_deque::traits
  {
    typedef mock_disposer disposer;
    typedef cds::atomicity::item_counter item_counter;
    typedef ci::michael_deque::stat<> stat;
    typedef cds::opt::v::sequential_consistent memory_model;
    typedef ci::michael_deque::base_hook< ci::opt::gc<gc_type>> hook;
  };
  typedef cds::intrusive::MichaelDeque< gc_type, base_item_type, traits > test_deque;

  std::vector<base_item_type> arr;
  arr.resize(100);
  {
    test_deque q;
    test(q, arr);
  }
  gc_type::scan();
  check_array(arr);
}

TEST_F(IntrusiveMichaelDeque_DHP, member_hook)
{
  typedef cds::intrusive::MichaelDeque< gc_type, member_item_type,
          typename ci::michael_deque::make_traits<
          ci::opt::disposer< mock_disposer >
          , ci::opt::hook< ci::michael_deque::member_hook<
          offsetof(member_item_type, hMember),
          ci::opt::gc<gc_type>
          >>
          >::type
          > test_deque;

  std::vector<member_item_type> arr;
  arr.resize(100);
  {
    test_deque q;
    test(q, arr);
  }
  gc_type::scan();
  check_array(arr);
}

TEST_F(IntrusiveMichaelDeque_DHP, member_hook_stat)
{
  struct traits : public ci::michael_deque::traits
  {
    typedef ci::michael_deque::member_hook <
    offsetof(member_item_type, hMember),
             ci::opt::gc<gc_type>
             > hook;
    typedef mock_disposer disposer;
    typedef cds::atomicity::item_counter item_counter;
    typedef ci::michael_deque::stat<> stat;
    typedef cds::opt::v::sequential_consistent memory_model;
  };
  typedef cds::intrusive::MichaelDeque< gc_type, member_item_type, traits > test_deque;

  std::vector<member_item_type> arr;
  arr.resize(100);
  {
    test_deque q;
    test(q, arr);
  }
  gc_type::scan();
  check_array(arr);
}

} // namespace

