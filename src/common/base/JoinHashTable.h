// Copyright (c) 2020 vesoft inc. All rights reserved.
//
// This source code is licensed under Apache 2.0 License.

#pragma once

#include <sys/types.h>

#include <cassert>
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "gtest/gtest_prod.h"

// Get the next power of 2 number by input n
static inline std::size_t nextPowerOf2(std::size_t n) {
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;
  return n;
}

// The hash table optimized for Join operation.
// Featuring:
//   1. Fixsized capacity without rehashing!
//   2. Linear probing for better cache friendliness.
//   3. Capacity is power of 2, to get modulo by bit operation.
template <typename Key,
          typename T,
          typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          template <typename> class Allocator = std::allocator>
class JoinHashTable final {
 public:
  using ValueType = std::pair<const Key, T>;
  //  using Allocator = Allocator<ValueType>;
  struct Bucket {
    Bucket() = default;
    explicit Bucket(ValueType &&value) : value_(new ValueType(std::move(value))) {}

    bool has_value() const {
      return value_ != nullptr;
    }

    const ValueType &value() const {
      return *value_;
    }

    ValueType &value() {
      return *value_;
    }

    std::unique_ptr<ValueType> value_{nullptr};
  };

  //  using Allocator = Allocator<ValueType>;
  // inline storage for small object and remote storage for large object
  using Entry = typename std::conditional<std::bool_constant<sizeof(T) <= 64>::value,
                                          std::optional<ValueType>,
                                          Bucket>::type;
  using EntryAllocator = Allocator<Entry>;

  static constexpr std::size_t kLoadFactor = 85;        // 85%
  static constexpr std::size_t kAlmostFullFactor = 90;  // 90%

  static inline ValueType *valuePointer(Entry &entry) {
    assert(entry.has_value());
    return &entry.value();
  }
  static inline const ValueType *valuePointer(const Entry &entry) {
    assert(entry.has_value());
    return &entry.value();
  }

  static inline ValueType &valueReference(Entry &entry) {
    assert(entry.has_value());
    return entry.value();
  }

  static inline const ValueType &valueReference(const Entry &entry) {
    assert(entry.has_value());
    return entry.value();
  }

  explicit JoinHashTable(std::size_t size_hint)
      : capacity_(cap(size_hint)),
        indexMask_(capacity_ - 1),
        size_(0),
        table_(alloc.allocate(capacity_ + 1 /* Dummy entry for end */)) {
    assert(capacity_ && !(capacity_ & (capacity_ - 1)));  // capacity must be power of 2
    for (std::size_t i = 0; i < capacity_; ++i) {
      alloc.construct(table_ + i, Entry());
    }
    // dummy end
    alloc.construct(table_ + capacity_, std::make_pair(Key(), T()));
  }
  JoinHashTable(const JoinHashTable &) = delete;
  JoinHashTable(JoinHashTable &&) = delete;

  ~JoinHashTable() {
    const std::size_t ele = capacity_ + 1;
    for (std::size_t i = 0; i < ele; ++i) {
      if (table_[i].has_value()) {
        alloc.destroy(table_ + i);
      }
    }
    alloc.deallocate(table_, ele);
  }

  template <typename EntryType, typename ValueType2>
  class IteratorImpl {
   public:
    //<
    typedef std::forward_iterator_tag iterator_category_t;
    typedef ValueType2 value_t;
    typedef std::ptrdiff_t difference_t;
    using pointer_t = ValueType2 *;
    using reference_t = ValueType2 &;
    using EntryPointer_t = EntryType *;
    using EntryReference_t = EntryType &;

    //< constructor
    IteratorImpl() {
      pointer_ = nullptr;
    }
    explicit IteratorImpl(EntryPointer_t init) {
      pointer_ = init;
    }

    //< dereference operator
    reference_t operator*() const {
      return pointer_->value();
    }
    pointer_t operator->() const {
      return &(pointer_->value());
    }

    //< succ
    IteratorImpl<EntryType, ValueType2> &operator++() {
      while (!(++pointer_)->has_value()) {
      }
      return *this;
    }
    IteratorImpl<EntryType, ValueType2> operator++(int) {
      IteratorImpl<EntryType, ValueType2> temp = *this;
      ++(*this);
      return temp;
    }

    //< equl
    bool operator!=(const IteratorImpl<EntryType, ValueType2> right) const {
      return pointer_ != right.pointer_;
    }
    bool operator==(const IteratorImpl<EntryType, ValueType2> right) const {
      return pointer_ == right.pointer_;
    }

    EntryPointer_t pointer_;
  };

  using Iterator = IteratorImpl<Entry, ValueType>;
  using ConstIterator = IteratorImpl<const Entry, const ValueType>;

  static inline Iterator constIterToMut(ConstIterator iter) {
    return Iterator(const_cast<Entry *>(iter.pointer_));
  }

  inline std::size_t index(const Key &key) const {
    const std::size_t indexNumber = Hash()(key) & indexMask_;  // hash % capacity
    assert(indexNumber < capacity_);
    return indexNumber;
  }

  template <typename _Key, typename... Args>
  std::pair<Iterator, bool> emplace(_Key &&key, Args &&... args) {
    assert(!almostFull());  // don't allowed
    if (almostFull()) {
      return std::make_pair(end(), false);
    }
    const std::size_t indexNumber = index(key);
    const std::size_t iteration = capacity_ + indexNumber;
    for (std::size_t i = indexNumber; i < iteration; ++i) {
      std::size_t modIndex = i & indexMask_;
      if (!table_[modIndex].has_value()) {
        alloc.construct(
            table_ + modIndex,
            std::pair<const Key, T>(std::forward<_Key>(key), std::forward<Args>(args)...));
        size_ += 1;
        return std::make_pair(Iterator(table_ + modIndex), true);
      } else {
        if (KeyEqual()(key, table_[modIndex].value().first)) {
          return std::make_pair(Iterator(table_ + modIndex), false);
        }
      }
    }
    return std::make_pair(end(), false);
  }

  ConstIterator find(const Key &key) const {
    const std::size_t indexNumber = index(key);
    const std::size_t iteration = capacity_ + indexNumber;
    for (std::size_t i = indexNumber; i < iteration; ++i) {
      std::size_t modIndex = i & indexMask_;
      auto r = findIndex(modIndex, key);
      if (r.first != end()) {
        return r.first;
      } else if (!r.second) {
        return end();
      }
    }
    return end();
  }
  Iterator find(const Key &key) {
    return constIterToMut(
        const_cast<const JoinHashTable<Key, T, Hash, KeyEqual, Allocator> *>(this)->find(key));
  }

  T &operator[](const Key &key) {
    auto r = emplace(key, T());
    assert(r.first != end());
    return r.first->second;
  }
  T &operator[](Key &&key) {
    auto r = emplace(std::move(key), T());
    assert(r.first != end());
    return r.first->second;
  }

  ConstIterator begin() const {
    for (std::size_t i = 0; i < capacity_; ++i) {
      if (table_[i].has_value()) {
        return ConstIterator(table_ + i);
      }
    }
    return end();
  }
  Iterator begin() {
    return constIterToMut(
        const_cast<const JoinHashTable<Key, T, Hash, KeyEqual, Allocator> *>(this)->begin());
  }

  Iterator end() {
    return Iterator(table_ + capacity_);
  }
  ConstIterator end() const {
    return ConstIterator(table_ + capacity_);
  }

  std::size_t size() const {
    return size_;
  }

  inline bool almostFull() const {
    return (size_ * kAlmostFullFactor / 100) > capacity_;
  }

 private:
  // \return ConstIterator the found iterator, end() means don't found
  // \return bool means whether continue finding
  std::pair<ConstIterator, bool> findIndex(const std::size_t i, const Key &key) const {
    assert(i < capacity_);
    if (table_[i].has_value()) {
      if (KeyEqual()(key, table_[i].value().first)) {
        return std::make_pair(ConstIterator(table_ + i), false);
      } else {
        return std::make_pair(end(), true);
      }
    } else {
      // Reach empty entry
      return std::make_pair(end(), false);
    }
  }
  std::pair<Iterator, bool> findIndex(const std::size_t i, const Key &key) {
    auto result =
        const_cast<const JoinHashTable<Key, T, Hash, KeyEqual, Allocator> *>(this)->findIndex(i,
                                                                                              key);
    return std::make_pair(constIterToMut(result.first), std::move(result.second));
  }

  static inline std::size_t cap(const std::size_t size_hint) {
    auto cap = nextPowerOf2(size_hint * 100 / kLoadFactor);
    return cap > 2 ? cap : 2;
  }

  FRIEND_TEST(JoinHashTableTest, Cap);

 private:
  EntryAllocator alloc;
  const std::size_t capacity_{0};  // capacity of the table
  // index mask, always equals to (capacity - 1)
  const std::size_t indexMask_{0};
  std::size_t size_{0};    // size of elements
  Entry *table_{nullptr};  // vector of elements
};
