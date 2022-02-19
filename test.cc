// Include order is
//   The h file for the tested module
//   Standard headers in alphabetical order
//   Other local headers

#include "inline-vector.h"

#include <cassert>
#include <iostream>
#include <vector>

static void test_fixed_static() {
  using V = ivec::FixedSizeInlineVector<int, 3>;
  static_assert(std::is_same_v<int, V::value_type>);
  static_assert(std::is_same_v<size_t, V::size_type>);
  static_assert(std::is_same_v<ptrdiff_t, V::difference_type>);
  static_assert(std::is_same_v<int&, V::reference>);
  static_assert(std::is_same_v<const int&, V::const_reference>);
  static_assert(std::is_same_v<int*, V::iterator>);
  static_assert(std::is_same_v<const int*, V::const_iterator>);
}

static void test_fixed_simple() {
  ivec::FixedSizeInlineVector<int, 3> a0;
  assert(a0.empty());
  assert(a0.size() == 0);
  assert(a0.max_size() == 3);
  a0.reserve(2); // does nothing
  assert(a0.size() == 0);
  assert(a0.capacity() == 3);
}

static void test_fixed_access() {
  ivec::FixedSizeInlineVector<int, 3> a;
  a.push_back(4);
  assert(a.at(0) == 4);
  a.push_back(5);
  assert(a.at(0) == 4);
  assert(a.at(1) == 5);
  a.at(0) = 3;
  assert(a.at(0) == 3);
  assert(a[0] == 3);
  const ivec::FixedSizeInlineVector<int, 3> &b = a;
  assert(b.size() == 2);
  assert(b.at(0) == 3);
  assert(b[0] == 3);

  // front
  assert(a.front() == a[0]);
  assert(&a.front() == &a[0]);

  // back
  assert(a.back() == a[1]);
  assert(&a.back() == &a[1]);

  // data
  assert(a.data() == &a[0]);
}

static void test_fixed_iterator() {
  ivec::FixedSizeInlineVector<int, 3> a;
  {
    std::vector<int> found;
    for (int& v : a) { found.push_back(v); }
    assert(found == std::vector<int>{});
  }
  a.push_back(4);
  {
    std::vector<int> found;
    for (int& v : a) { found.push_back(v); }
    assert(found == std::vector<int>{4});
  }
  a.push_back(5);
  {
    std::vector<int> found;
    for (int& v : a) { found.push_back(v); }
    assert(found == (std::vector<int>{4, 5}));
  }
  a.push_back(6);
  {
    std::vector<int> found;
    for (int& v : a) { found.push_back(v); }
    assert(found == (std::vector<int>{4, 5, 6}));
  }
  {
    std::vector<int> found;
    for (int& v : a) { found.push_back(v); }
    assert(found == (std::vector<int>{4, 5, 6}));
  }

  const ivec::FixedSizeInlineVector<int, 3> &b = a;
  {
    std::vector<int> found;
    for (const int& v : b) { found.push_back(v); }
    assert(found == (std::vector<int>{4, 5, 6}));
  }

  {
    std::vector<int> found;
    for (auto r = a.rbegin(); r != a.rend(); ++r) {
      found.push_back(*r);
    }
    assert(found == (std::vector<int>{6, 5, 4}));
  }
  {
    std::vector<int> found;
    for (auto r = a.crbegin(); r != a.crend(); ++r) {
      found.push_back(*r);
    }
    assert(found == (std::vector<int>{6, 5, 4}));
  }
  {
    std::vector<int> found;
    for (auto r = b.rbegin(); r != b.rend(); ++r) {
      found.push_back(*r);
    }
    assert(found == (std::vector<int>{6, 5, 4}));
  }
}

static void test_fixed_compare() {
  ivec::FixedSizeInlineVector<int, 3> a0;
  assert(a0 == a0);
  assert(a0 <= a0);
  assert(!(a0 < a0));
  assert(a0 >= a0);
  assert(!(a0 > a0));

  ivec::FixedSizeInlineVector<int, 3> b1;
  b1.push_back(3);
  assert(a0 != b1);
  assert(!(a0 == b1));
  assert(a0 < b1);
  assert(a0 <= b1);
  assert(!(a0 > b1));
  assert(!(a0 >= b1));

  assert(b1 != a0);
  assert(!(b1 == a0));
  assert(!(b1 < a0));
  assert(!(b1 <= a0));
  assert(b1 > a0);
  assert(b1 >= a0);
}

static size_t destruction_count = 0;

struct CountDestructions {
  explicit CountDestructions(bool count_destruction = false)
      :count_destruction(count_destruction) {}
  ~CountDestructions() {
    if (count_destruction){
      ++destruction_count;
    }
  }
  bool count_destruction;
};

static void test_clear() {
  {
    ivec::FixedSizeInlineVector<CountDestructions, 3> b;
    b.push_back(CountDestructions{true});
    b.push_back(CountDestructions{true});
    destruction_count = 0;
  }
  assert(destruction_count == 2);
  {
    ivec::FixedSizeInlineVector<CountDestructions, 3> b;
    b.push_back(CountDestructions{true});
    b.push_back(CountDestructions{true});
    destruction_count = 0;
    // make sure the clear actually destructs the items that were cleared with
    // default items (which won't count).
    b.clear();
    assert(destruction_count == 2);
    assert(b.empty());
  }
  assert(destruction_count == 2);
}

int main() {
  test_fixed_static();
  test_fixed_simple();
  test_fixed_access();
  test_fixed_iterator();
  test_fixed_compare();
  test_clear();
}
