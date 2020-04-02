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

#ifndef CDSUNIT_DEQUE_TEST_INTRUSIVE_MICHAEL_DEQUE_H
#define CDSUNIT_DEQUE_TEST_INTRUSIVE_MICHAEL_DEQUE_H

#include <cds_test/check_size.h>

namespace cds_test
{

class intrusive_michael_deque : public ::testing::Test
{
protected:
  template <typename Base>
  struct base_hook_item : public Base
  {
    int nVal;
    int nDisposeCount;

    base_hook_item()
      : nDisposeCount(0)
    {}

    base_hook_item(base_hook_item const& s)
      : nVal(s.nVal)
      , nDisposeCount(s.nDisposeCount)
    {}
  };

  template <typename Member>
  struct member_hook_item
  {
    int nVal;
    int nDisposeCount;
    Member hMember;

    member_hook_item()
      : nDisposeCount(0)
    {}

    member_hook_item(member_hook_item const& s)
      : nVal(s.nVal)
      , nDisposeCount(s.nDisposeCount)
    {}
  };

  struct mock_disposer
  {
    template <typename T>
    void operator()(T* p)
    {
      ++p->nDisposeCount;
    }
  };

  template <typename Deque, typename Data>
  void test(Deque& q, Data& arr)
  {
    typedef typename Deque::value_type value_type;
    size_t nSize = arr.size();

    value_type* pv;
    for (size_t i = 0; i < nSize; ++i)
      arr[i].nVal = static_cast<int>(i);

    ASSERT_TRUE(q.empty());
    ASSERT_CONTAINER_SIZE(q, 0);

    // pop from empty deque
    pv = q.pop_right();
    ASSERT_TRUE(pv == nullptr);
    ASSERT_TRUE(q.empty());
    ASSERT_CONTAINER_SIZE(q, 0);

    pv = q.pop_left();
    ASSERT_TRUE(pv == nullptr);
    ASSERT_TRUE(q.empty());
    ASSERT_CONTAINER_SIZE(q, 0);

    // push/pop test
    for (size_t i = 0; i < nSize; ++i)
      {
        if (i & 1)
          q.push_right(arr[i]);
        else
          q.push_left(arr[i]);
        ASSERT_FALSE(q.empty());
        ASSERT_CONTAINER_SIZE(q, i + 1);
      }

    for (size_t i = 0; i < nSize; ++i)
      {
        ASSERT_FALSE(q.empty());
        ASSERT_CONTAINER_SIZE(q, nSize - i);
        if (i & 1)
          pv = q.pop_left();
        else
          pv = q.pop_right();
        ASSERT_FALSE(pv == nullptr);
        ASSERT_EQ(pv->nVal, static_cast<int>(nSize - i - 1));
      }
    ASSERT_TRUE(q.empty());
    ASSERT_CONTAINER_SIZE(q, 0);

    //test that items were disposed 1 time
    Deque::gc::scan();
    for (size_t i = 0; i < nSize; ++i)
      {
        ASSERT_EQ(arr[i].nDisposeCount, 1);
      }
    ASSERT_EQ(arr[nSize].nDisposeCount, 0);

    // clear test
    for (size_t i = 0; i < nSize; ++i)
      q.push_right(arr[i]);

    ASSERT_FALSE(q.empty());
    ASSERT_CONTAINER_SIZE(q, nSize);

    q.clear();
    ASSERT_TRUE(q.empty());
    ASSERT_CONTAINER_SIZE(q, 0);

    //test that items were disposed 2 times
    Deque::gc::scan();
    for (size_t i = 0; i < nSize; ++i)
      {
        ASSERT_EQ(arr[i].nDisposeCount, 2) << "i=" << i;
      }
  }
};

} // namespace cds_test

#endif // CDSUNIT_DEQUE_TEST_INTRUSIVE_MICHAEL_DEQUE_H
