#ifndef INLINE_VECTOR_H_
#define INLINE_VECTOR_H_

#include <cassert>
#include <cstddef>
#include <iterator>
#include <utility>
#include <variant>
#include <vector>

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
  const value_type* data() const noexcept { return values_; }

  iterator begin() { return &values_[0]; }
  const_iterator begin() const { return cbegin(); }
  const_iterator cbegin() const { return &values_[0]; }

  iterator end() { return &values_[size_]; }
  const_iterator end() const { return cend(); }
  const_iterator cend() const { return &values_[size_]; }

  reverse_iterator rbegin() { return std::reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return crbegin(); }
  const_reverse_iterator crbegin() const { return std::reverse_iterator(cend()); }

  reverse_iterator rend() { return std::reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return crend(); }
  const_reverse_iterator crend() const { return std::reverse_iterator(cbegin()); }

  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }
  size_t max_size() const { return vec_capacity; }
  // Does nothing.
  void reserve(size_t) const {}
  size_t capacity () const { return vec_capacity; }
  // Does nothing.
  void shrink_to_fit(size_t) const {}
  void clear() {
    // Set all the elements to the default constructed value.
    while (size_ > 0) {
      values_[--size_] = T();
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
  void pop_back() {
    assert(size_ > 0);
    back() = T(); // set the deleted item to the default
    --size_;
  }
  // TODO: Implement emplace_back
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

template <class T, size_t vec_capacity>
class InlineVector {
  template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
  template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

  using Fixed = FixedSizeInlineVector<T, vec_capacity>;
  using Vector = std::vector<T>;
 public:
  using value_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = value_type*;
  using const_iterator = const value_type*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  reference at(size_type pos) { 
    return std::visit([&](auto& arg) { return arg.at(pos); }, v_); 
  }
  const_reference at(size_type pos) const { 
    return std::visit([&](auto& arg) { return arg.at(pos); }, v_); 
  }

  void push_back(value_type value) {
    return std::visit(overloaded {
        [&](Fixed& f) {
          if (f.size() == f.capacity()) {
            Vector vec;
            for (auto &val : f) {
              vec.push_back(std::move(val));
            }
            vec.push_back(std::move(value));
            v_ = vec;
          } else {
            f.push_back(std::move(value));
          }
        },
        [&](Vector& vec){
          vec.push_back(std::move(value));
        }
      }, v_);
  }
  void pop_back() {
    return std::visit([&](auto& arg) { return arg.pop_back(); }, v_); 
  }

  reference operator[](size_type pos) {
    return std::visit([&](auto& arg)->reference { return arg[pos]; }, v_); 
  }
  const_reference operator[](size_type pos) const {
    return std::visit([&](auto& arg)->const_reference { return arg[pos]; }, v_); 
  }

  reference front() { 
    return std::visit([](auto& arg)->reference { return arg.front(); }, v_); 
  }
  const_reference front() const {
    return std::visit([](auto& arg)->const_reference { return arg.front(); },
                      v_); 
  }

  reference back() { 
    return std::visit([](auto& arg)->reference { return arg.back(); }, v_); 
  }
  const_reference back() const {
    return std::visit([](auto& arg)->const_reference { return arg.back(); },
                      v_); 
  }

  value_type* data() noexcept { 
    return std::visit([](auto& arg) { return arg.data(); }, v_); 
  }
  const value_type* data() const noexcept {
    return std::visit([](auto& arg) { return arg.data(); }, v_); 
  }

  iterator begin() { 
    // cannot just use a Vector::begin because it's not just a pointer.
    return std::visit(overloaded {
        [](Fixed& arg) { return arg.begin(); },
        [](Vector& arg) { return &arg[0]; }
      },
      v_); 
  }
  const_iterator begin() const {
    return cbegin();
  }
  const_iterator cbegin() const {
    return const_cast<InlineVector*>(this)->begin(); 
  }

  iterator end() { 
    return std::visit(overloaded {
        [](Fixed& arg) { return arg.end(); },
        [](Vector& arg) { return 1 + &arg.back(); }
      },
      v_); 
  }
  const_iterator end() const {
    return cend();
  }
  const_iterator cend() const {
    return const_cast<InlineVector*>(this)->end();
  }

  reverse_iterator rbegin() { return std::reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return crbegin(); }
  const_reverse_iterator crbegin() const { return std::reverse_iterator(cend()); }

  reverse_iterator rend() { return std::reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return crend(); }
  const_reverse_iterator crend() const { return std::reverse_iterator(cbegin()); }

  bool empty() const {
    return std::visit([](auto& arg) { return arg.empty(); }, v_); 
  }

  size_t size() const {
    return std::visit([](auto& arg) { return arg.size(); }, v_); 
  }

  size_t capacity() const {
    return std::visit([](auto& arg) { return arg.capacity(); }, v_); 
  }

  void clear() {
    // We don't convert back to the short representation.
    return std::visit([](auto& arg) { return arg.clear(); }, v_); 
  }
  
  bool is_inlined() const {
    return std::holds_alternative<Fixed>(v_);
  }
 private:
  std::variant<Fixed, Vector> v_;
};

}  // namespace ivec
#endif  // INLINE_VECTOR_H_
