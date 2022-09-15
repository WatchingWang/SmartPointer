#pragma once

#include "PtrBase.h"

template <typename T>
class WeakPtr : public PtrBase<T> {
 public:
  WeakPtr() = default;
  template <typename T2>
  WeakPtr(const PtrBase<T2>& other) {
    this->WeaklyConstructFrom(other);
  }

  bool IsExpired() { return this->ref_->UseCount() == 0 ? true : false; }

  SharedPtr<T> Lock() {
    SharedPtr<T> ret;
    ret.ConstructFromWeak(*this);
    return ret;
  }
  ~WeakPtr() { this->ref_->Decwref(); }
};