// Include order is
//   The h file for the tested module
//   Standard headers in alphabetical order
//   Other local headers

#include "inline-vector.h"

#include <cassert>

static void test_fixed_size_inline_vector() {
  ivec::FixedSizeInlineVector<int, 3> a;
  assert(a.empty());
}

int main() {
  test_fixed_size_inline_vector();
}
