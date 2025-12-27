#pragma once

#include <cassert>
#include <cstdint>
#include <vector>

using Id = uint32_t;

template <typename T>
class SparseSet {
 public:
  // Constants
  static constexpr uint32_t kInvalid = UINT32_MAX;

  // Insert by forwarding arguments
  template <typename... Args>
  Id Emplace(Args&&... args) {
    Id id = AllocateId();
    uint32_t dense_index = static_cast<uint32_t>(dense_.size());

    dense_.emplace_back(std::forward<Args>(args)...);
    dense_to_id_.push_back(id);
    sparse_[id] = dense_index;

    return id;
  }

  // Insert by copy
  Id Insert(const T& value) { return Emplace(value); }

  // Remove element using end-swap erase
  void Remove(Id id) {
    assert(Contains(id));

    uint32_t remove_index = sparse_[id];
    uint32_t last_index = static_cast<uint32_t>(dense_.size() - 1);

    if (remove_index != last_index) {
      dense_[remove_index] = std::move(dense_[last_index]);

      Id moved_id = dense_to_id_[last_index];
      dense_to_id_[remove_index] = moved_id;
      sparse_[moved_id] = remove_index;
    }

    dense_.pop_back();
    dense_to_id_.pop_back();

    sparse_[id] = kInvalid;
    free_ids_.push_back(id);
  }

  bool Contains(Id id) const { return id < sparse_.size() && sparse_[id] != kInvalid; }

  T& Get(Id id) {
    assert(Contains(id));
    return dense_[sparse_[id]];
  }

  const T& Get(Id id) const {
    assert(Contains(id));
    return dense_[sparse_[id]];
  }

  T& operator[](Id id) {
    assert(Contains(id));
    return dense_[sparse_[id]];
  }

  const T& operator[](Id id) const {
    assert(Contains(id));
    return dense_[sparse_[id]];
  }

  std::vector<T> Get(std::span<const Id> ids) const {
    std::vector<T> result;
    result.reserve(ids.size());

    for (Id id : ids) {
      assert(Contains(id));
      result.push_back(dense_[sparse_[id]]);
    }

    return result;
  }

  // Use for building GPU index buffers
  uint32_t DenseIndex(Id id) const {
    assert(Contains(id));
    return sparse_[id];
  }

  uint32_t DenseCount() const { return static_cast<uint32_t>(dense_.size()); }

  const std::vector<T>& Dense() const { return dense_; }

 private:
  Id AllocateId() {
    if (!free_ids_.empty()) {
      Id id = free_ids_.back();
      free_ids_.pop_back();
      return id;
    }
    Id id = static_cast<Id>(sparse_.size());
    sparse_.push_back(kInvalid);
    return id;
  }

  std::vector<T> dense_;
  std::vector<Id> dense_to_id_;
  std::vector<uint32_t> sparse_;
  std::vector<Id> free_ids_;
};
