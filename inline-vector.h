#ifndef INLINE_VECTOR_H_
#define INLINE_VECTOR_H_

#include <cstddef>
#include <iterator>

namespace ivec {

namespace ivec_internal {

}  // namespace ivec_internal

// A vector that can grow only to a particular capacity.
template <class T, size_t capacity>
class FixedSizeInlineVector {
 public:
  // Standard member types provided by std::vector
  using value_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = ivec_internal::PointerIteratorImplementation<T, false>;
  using const_iterator = PointerIteratorImplementation<T, true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  // Returns a reference to the pos'th value.  Unlike std::vector, doesn't throw
  // exceptions.  Referencing beyond size() is undefined behavior.
  reference at(size_type pos) { return values_[pos]; }
  const_reference at(size_type pos) const { return values_[pos]; }

  reference operator[](size_type pos) { return values_[pos]; }
  const_reference operator[](size_type pos) const { return values_[pos]; }

  reference front() { return values_[0]; }
  const_reference front() const { return values_[0]; }

  reference back() { return values_[size_ - 1]; }
  const_reference back() const { return values_[size_ - 1]; }

  value_type* data() noexcept { return values_; }
  value_type* data() const noexcept { return values_; }

  iterator begin() { return iterator(&values_[0]); }

  bool empty() const { return size_ == 0; }
 private: 
  size_t size_ = 0;
  value_type values_[capacity];
};

template <class T, bool is_const>
class FixedSizeInlineIteratorImplementation {
  using Iter = FixedSizeInlineIteratorImplementation;
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = T;
  using difference_type = ptrdiff_t;
  using pointer = typename std::conditional<is_const,
                                            const value_type*,
                                            value_type*>::type;
  using reference = typename std::conditional<is_const,
                                              const value_type&,
                                              value_type&>::type;
  // Conversion operator from any iterator to const_iterator.  This allows you
  // do do pass an iterator to a method that accepts a const_iterator without an
  // explicit conversion.  Most implicit conversions are frowned upon (e.g., by
  // the Google style guide), but this conversion is idiomatic in C++.
  operator FixedSizeInlineIteratorImplementation<T, true>() const { 
    return FixedSizeInlineIteratorImplementation<T, true>(pointer_);
  }
  // Prefix increment / decrement
  Iter& operator++() {
    ++pointer_;
    return *this;
  }
  Iter& operator--() {
    ++pointer_;
    return *this;
  }
  // Postfix increment / decrement
  Iter operator++(int) {
    Iter old = *this;
    ++pointer_;
    return old;
  }
  Iter operator--(int) {
    Iter old = *this;
    --pointer_;
    return old;
  }
  Iter& operator+=(ptrdiff_t n) {
    pointer_ += n;
    return *this;
  }
  Iter& operator-=(ptrdiff_t n) {
    pointer_ -= n;
    return *this;
  }
  friend Iter operator+(Iter a, ptrdiff_t n) {
    a.pointer_ += n;
    return a;
  }
  friend Iter operator+(ptrdiff_t n, Iter a) {
    a.pointer_ += n;
    return a;
  }
  friend Iter operator-(Iter a, ptrdiff_t n) {
    a.pointer_ -= n;
    return a;
  }
  friend ptrdiff_t  operator-(Iter a, Iter b) {
    return a.pointer_ - b.pointer;
  }
  reference operator*() const { return *pointer_; }
  pointer operator->() const { return pointer_; }
  reference& operator[](size_t i) {
    return *(pointer_ + i);
  }
  friend bool operator<(Iter a, Iter b) {
    return a.pointer_ < b.pointer;
  }
  friend bool operator>(Iter a, Iter b) {
    return a.pointer_ > b.pointer;
  }
  friend bool operator<=(Iter a, Iter b) {
    return a.pointer_ <= b.pointer;
  }
  friend bool operator>=(Iter a, Iter b) {
    return a.pointer_ >= b.pointer;
  }
  friend bool operator==(Iter a, Iter b) {
    return a.pointer_ == b.pointer;
  }
 private:
  pointer pointer_;
};

}  // namespace ivec
#endif  // INLINE_VECTOR_H_
