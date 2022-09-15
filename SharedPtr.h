#pragma once

#include "PtrBase.h"

template <typename T>
class SharedPtr : public PtrBase<T> {
 public:
  SharedPtr() = default;
  template <typename OtherT>
  SharedPtr(OtherT* other) {
    this->ref_ = new RefCountResource<OtherT*, DefaultDeleter<OtherT>>(
        other, DefaultDeleter<OtherT>());
    this->ptr_ = other;
  }

  SharedPtr(const SharedPtr& other) { this->CopyConstructFrom(other); }

  template <typename T2>
  SharedPtr(const SharedPtr<T2>& other) {
    this->CopyConstructFrom(other);
  }

  template <typename T2>
  SharedPtr(SharedPtr<T2>&& other) {
    this->MoveConstructFrom(std::move(other));
  }

  T& operator*() { return *this->Get(); }

  T* operator->() { return this->Get(); }
  ~SharedPtr() { this->Decref(); }

  template <typename T, typename... Args>
  friend SharedPtr<T> make_shared(Args&&... args);
};

template <typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
  SharedPtr<T> ret;
  auto ref = new RefCountObj<T>(std::forward<Args>(args)...);
  ret.ref_ = ref;
  ret.ptr_ = &(ref->value_);
  return ret;
}