// Include order is
//   The h file for the tested module
//   Standard headers in alphabetical order
//   Other local headers

#include "inline-vector.h"

#include <cassert>

static void test_fixed_size_inline_vector() {
  ivec::FixedSizeInlineVector<int, 3> a0;
  assert(a0.empty());
  assert(a0.size() == 0);
  assert(a0.max_size() == 3);
  a0.reserve(2); // does nothing
  assert(a0.size() == 0);
  assert(a0.capacity() == 3);
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

int main() {
  test_fixed_size_inline_vector();
}
