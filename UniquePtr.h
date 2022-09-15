#pragma once

#include "PtrBase.h"

template <typename T, typename deleter = DefaultDeleter<T>>
class UniquePtr {
 public:
  UniquePtr(T* ptr) : ptr_(ptr) {}
  template <typename RT>
  UniquePtr(RT* ptr) : ptr_(ptr) {}

  UniquePtr(UniquePtr&& rhs) {
    ptr_ = rhs.Get();
    rhs.ptr_ = nullptr;
    del = rhs.del;
  }

  template <typename RT>
  UniquePtr(UniquePtr<RT>&& rhs) {
    ptr_ = rhs.ptr_;
    rhs.ptr_ = nullptr;
    del = rhs.del;
  }

  T* Get() { return ptr_; }

  ~UniquePtr() { del(ptr_); }

 private:
  template <class, class>
  friend class UniquePtr;

  deleter del;
  T* ptr_ = nullptr;
};

template <typename T>
UniquePtr<T> make_unique(T* ptr) {
  return UniquePtr<T>(ptr);
}

template <typename L, typename R>
UniquePtr<L> static_pointer_cast(UniquePtr<R> ptr) {
  return ptr;
}
