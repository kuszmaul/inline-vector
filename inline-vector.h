#ifndef INLINE_VECTOR_H_
#define INLINE_VECTOR_H_

#include <cassert>
#include <cstddef>
#include <iterator>
#include <utility>

namespace ivec {

// A vector that can grow only to a particular capacity.
template <class T, size_t vec_capacity>
class FixedSizeInlineVector {
 public:
  // Standard member types provided by std::vector
  using value_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = value_type*;
  using const_iterator = const value_type*;
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

  iterator begin() { return &values_[0]; }
  const_iterator begin() const { return cbegin(); }
  const_iterator cbegin() const { return &values_[0]; }

  iterator end() { return &values_[size_]; }
  const_iterator end() const { return cend(); }
  const_iterator cend() const { return &values_[size_]; }

  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }
  size_t max_size() const { return vec_capacity; }
  // Does nothing.
  void reserve(size_t) const {}
  size_t capacity () const { return vec_capacity; }
  // Does nothing.
  void shrink_to_fit(size_t) const {}
  void clear() {
    // Set all the elements to the default constructed vale.
    while (size_ > 0) {
      T tmp;
      --size_;
      std::swap(tmp, values_[size_]);
    }
  }
  iterator insert(const_iterator pos, const T& value) {
    iterator iter;
    for (iter = values_[vec_capacity]; iter != pos + 1; --iter) {
      *(iter - 2) = std::move(*(iter - 1));
    }
    *(iter - 1) = std::move(value);
    return iter-1;
  }
  iterator insert(const_iterator pos, T&& value) {
    iterator iter;
    for (iter = values_[vec_capacity]; iter != pos + 1; --iter) {
      *(iter-2) = std::move(*(iter-1));
    }
    *(iter-1) = std::forward(value);
    return iter-1;
  }
  iterator insert(const_iterator pos, size_t count, const T& value) {
    iterator iter;
    for (iter = values_[vec_capacity]; iter > pos + count; --iter) {
      *(iter - 1 - count) = std::move(*(iter - count));
    }
    while (iter > pos) {
      *(iter - 1) = value;
      --iter;
    }
    return iter;
  }
  // TODO: Implement cases 4 and 5 of insert
  // TODO: Implement emplace
  // TODO: Implement erase
#if 0
  void push_back(const value_type& value) {
    assert(size_ < capacity());
    values_[size_++] = value;
  }
  void push_back(value_type&& value) {
    assert(size_ < capacity());
    values_[size_++] = std::forward(value);
  }
#endif
  void push_back(value_type value) {
    assert(size_ < capacity());
    values_[size_++] = std::move(value);
  }
  // TODO: Implement emplace_back, pop_back
  void resize(size_t count) {
    assert(count < capacity());
    if (size_ < count) {
      // All the intermediate values are already value initialized.
      size_ = count;
    } else {
      while (size_ > count) {
        T tmp;
        --size_;
        std::swap(tmp, values_[size_]);
      }
    }
  };
  // Unlike std::vector::swap, swapping these elements does invoke swap operations on individual elements.
#if 0
  void swap(FixedSizeInlineVector& other) {
    std::swap(size_, other.size_);
    size_t max = std::max(size_, other.size_);
    for (size_t i = 0; i < max; ++i) {
      std::swap(values_[i], other.values_[i]);
    }
  }
#endif
  friend bool operator==(const FixedSizeInlineVector& a, 
                         const FixedSizeInlineVector& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
      if (!(a[i] == b[i])) return false;
    }
    return true;
  }
  friend std::strong_ordering operator<=>(const FixedSizeInlineVector& a, 
                                          const FixedSizeInlineVector& b) {
    return std::lexicographical_compare_three_way(a.begin(), a.end(),
                                                  b.begin(), b.end());
  }

 private: 
  size_t size_ = 0;
  value_type values_[vec_capacity];
};

}  // namespace ivec
#endif  // INLINE_VECTOR_H_
