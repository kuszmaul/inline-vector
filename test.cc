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

#if 0
static size_t object_count = 0;
static size_t total_weight = 0;

class WeightedObject {
 public:
  WeightedObject(size_t weight = 0) :weight(weight) {
    ++object_count;
    total_weight += weight;
    std::cout << "construct oc=" << object_count << std::endl;
  }
  // copy constructor
  WeightedObject(const WeightedObject& v) :weight(v.weight) {
    ++object_count;
    total_weight += v.weight;
    std::cout << "cc " << v.weight << " oc=" << object_count << std::endl;
  }
  // move constructor
  WeightedObject(WeightedObject&& v) {
    weight = v.weight;
    total_weight += weight;
  }
  // copy assignment
  WeightedObject& operator=(const WeightedObject& v) {
    std::cout << "operator= old_weight=" << weight << std::endl;
    total_weight -= weight;
    weight = v.weight;
    total_weight += weight;
    std::cout << " operator= new oc=" << object_count << std::endl;
    return *this;
  }
  ~WeightedObject() {
    --object_count;
    total_weight -= weight;
    std::cout << "Destruct oc=" << object_count << std::endl;
  }
 private:
  size_t weight;
};

static void test_fixed_clear() {
  {
    ivec::FixedSizeInlineVector<WeightedObject, 3> b;
    assert(object_count == 3);
    assert(total_weight == 0);
    std::cout << "Doing push back 10" << std::endl;
    b.push_back(WeightedObject(10));
    std::cout << "Doing push back 12" << std::endl;
    b.push_back(WeightedObject(12));
    assert(object_count == 3);
    assert(total_weight == 22);
  }
  assert(object_count == 0);
  assert(total_weight == 0);
  {
    ivec::FixedSizeInlineVector<WeightedObject, 3> b;
    b.push_back(WeightedObject(10));
    b.push_back(WeightedObject(12));
    // make sure the clear actually destructs the items that were cleared with
    // default items (which won't count).
    std::cout << "just before clear: oc=" << object_count << " tw=" << total_weight << std::endl;
    b.clear();
    std::cout << "after oc=" << object_count << " tw=" << total_weight << std::endl;
    assert(object_count == 3);
    assert(total_weight == 0);
    assert(b.empty());
  }
  assert(object_count == 0);
}
#endif

// Makes sure that clear() (and push_back) reset the unused slots to default
// initialization.
static void test_fixed_clear() {
  ivec::FixedSizeInlineVector<std::string, 3> b;
  // This is a transparent-box test: We know we can access off the end since the
  // capacity is 3.
  assert(b.at(0).empty());
  assert(b.at(1).empty());
  assert(b.at(2).empty());
  b.push_back("a");
  b.push_back("b");
  b.push_back("c");
  assert(b.at(2) == "c");
  b.pop_back();
  assert(b.at(2).empty());
  assert(!b.at(1).empty());
  b.clear();
  assert(b.at(1).empty());
  assert(b.at(0).empty());
}

static void test_inline_simple() {
  ivec::InlineVector<int, 3> v;
  const ivec::InlineVector<int, 3>& cv = v;
  assert(v.size() == 0);
  assert(v.empty());

  v.push_back(1);
  assert(v.size() == 1);
  assert(v[0] == 1);
  assert(cv[0] == 1);
  assert(v.front() == 1);
  assert(cv.front() == 1);
  assert(v.data()[0] == 1);
  assert(!v.empty());

  {
    std::vector<int> found;
    for (auto x : v) found.push_back(x);
    assert(found == std::vector<int>{1});
  }
  {
    std::vector<int> found;
    for (auto x : cv) found.push_back(x);
    assert(found == std::vector<int>{1});
  }
  {
    std::vector<int> found;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
      found.push_back(*it);
    }
    assert(found == std::vector<int>{1});
  }
  {
    std::vector<int> found;
    for (auto it = cv.rbegin(); it != cv.rend(); ++it) {
      found.push_back(*it);
    }
    assert(found == std::vector<int>{1});
  }


  v.push_back(2);
  assert(v.size() == 2);

  v.push_back(3);
  assert(v.back() == 3);
  assert(cv.back() == 3);
  assert(v.size() == 3);
  v.push_back(4);
  assert(v.size() == 4);
  assert(v[0] == 1);
  assert(cv[0] == 1);
  assert(v[1] == 2);
  assert(v[2] == 3);
  assert(v[3] == 4);
  assert(v.front() == 1);
  assert(cv.front() == 1);
  assert(v.back() == 4);
  assert(cv.back() == 4);
  assert(v.data()[0] == 1);
  assert(cv.data()[3] == 4);
  assert(cv.data()[3] == 4);

  {
    std::vector<int> found;
    for (auto x : v) found.push_back(x);
    assert(found == (std::vector<int>{1, 2, 3, 4}));
  }
  {
    std::vector<int> found;
    for (auto x : cv) found.push_back(x);
    assert(found == (std::vector<int>{1, 2, 3, 4}));
  }
  {
    std::vector<int> found;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
      found.push_back(*it);
    }
    assert(found == (std::vector<int>{4, 3, 2, 1}));
  }
  {
    std::vector<int> found;
    for (auto it = cv.rbegin(); it != cv.rend(); ++it) {
      found.push_back(*it);
    }
    assert(found == (std::vector<int>{4, 3, 2, 1}));
  }

  assert(!v.empty());

  assert(!v.is_inlined());
  assert(v.capacity() >= 4);
  size_t old_capacity = v.capacity();
  v.clear();
  assert(v.empty());
  assert(v.capacity() == old_capacity);

  v.push_back(100);
  v.push_back(101);
  v.push_back(102);
  v.push_back(103);
  v.pop_back();
}

int main() {
  test_fixed_static();
  test_fixed_simple();
  test_fixed_access();
  test_fixed_iterator();
  test_fixed_compare();
  test_fixed_clear();
  test_inline_simple();
}
