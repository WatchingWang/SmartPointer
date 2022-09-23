#pragma once

#include "PtrBase.h"

template <typename T, typename Del = DefaultDeleter<T>>
class UniquePtr {
 public:
  UniquePtr(T* ptr) : pair_(ZeroVariadicArgs(), ptr) {}
  template <typename RT>
  UniquePtr(RT* ptr) : pair_(ZeroVariadicArgs(), ptr) {}

  UniquePtr(UniquePtr&& rhs)
      : pair_(OneVariadicArgs(), rhs.GetDeleter(), rhs.Release()) {}

  template <typename RT>
  UniquePtr(UniquePtr<RT>&& rhs)
      : pair_(OneVariadicArgs(), rhs.GetDeleter(), rhs.Release()) {}

  Del GetDeleter() { return pair_.GetFirst(); }

  T* Get() { return pair_.val2_; }

  T& operator*() { return *(pair_.val2_); }

  T* operator->() { return pair_.val2_; }

  T* Release() {
      auto ret = pair_.val2_;
      pair_.val2_ = nullptr;
      return ret;
  }

  operator bool() const { return static_cast<bool>(pair_.val2_); }

  void Reset(T* other = nullptr) {
	  if (pair_.val2_)
	  {
		  pair_.GetFirst()(pair_.val2_);
	  }
	  pair_.val2_ = other;
  }

  ~UniquePtr() { Reset(); }

 private:
  CompressedPair<Del, T*> pair_;
};

template <typename T>
UniquePtr<T> make_unique(T* ptr) {
  return UniquePtr<T>(ptr);
}

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
  return UniquePtr<T>(new T(std::forward<Args>(args)...));
}
