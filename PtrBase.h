#pragma once
#include <atomic>

struct ZeroVariadicArgs {};

struct OneVariadicArgs {};

template <typename T>
class DefaultDeleter {
 public:
  void operator()(T* ptr) { delete ptr; }
};

template <typename T1, typename T2>
class CompressedPair final : private T1 {
 public:
  T2 val2_;

  template <typename... Other2>
  explicit CompressedPair(ZeroVariadicArgs, Other2&&... val2)
      : T1(), val2_(std::forward<Other2>(val2)...) {}

  template <typename Other1, typename... Other2>
  explicit CompressedPair(OneVariadicArgs, Other1&& val1, Other2&&... val2)
      : T1(std::forward<Other1>(val1)), val2_(std::forward<Other2>(val2)...) {}

  T1& GetFirst() { return static_cast<T1&>(*this); }
};

class RefCountBase {
 public:
  virtual void Destroy() = 0;
  virtual void DeleteThis() = 0;

  void Incwref() { weaks_.fetch_add(1); }
  void Incref() {
    Incwref();
    uses_.fetch_add(1);
  }
  void Decwref() {
    if (weaks_.fetch_sub(1) == 1) {
      DeleteThis();
    }
  }

  void Decref() {
    if (uses_.fetch_sub(1) == 1) {
      Destroy();
    }
    Decwref();
  }

  uint64_t UseCount() { return uses_.load(); }
  uint64_t WeakCount() { return weaks_.load(); }

  RefCountBase() = default;
  RefCountBase(const RefCountBase&) = delete;
  RefCountBase& operator=(const RefCountBase&) = delete;

  virtual ~RefCountBase() {}

 private:
  std::atomic_uint64_t weaks_{1};
  std::atomic_uint64_t uses_{1};
};

template <typename T>
class RefCount : public RefCountBase {
 public:
  RefCount(T* ptr) : RefCountBase(), ptr_(ptr) {}
  void Destroy() override { delete ptr_; }

  void DeleteThis() override { delete this; }

 private:
  T* ptr_;
};

template <typename Resource, typename Deleter>
class RefCountResource : public RefCountBase {
 public:
  RefCountResource(Resource res, Deleter dt)
      : RefCountBase(), pair_(OneVariadicArgs(), std::move(dt), res) {}

  void Destroy() override { pair_.GetFirst()(pair_.val2_); }

  void DeleteThis() override { delete this; }

 private:
  CompressedPair<Deleter, Resource> pair_;
};

template <typename T>
class RefCountObj : public RefCountBase {
 public:
  template <typename... Types>
  RefCountObj(Types&&... args) : RefCountBase() {
    ::new (&value_) T(std::forward<Types>(args)...);
  }

  void Destroy() override { value_.~T(); }

  void DeleteThis() override { delete this; }

  T value_;
};


template <typename T>
class PtrBase {
 public:
  using element_type = T;

  void Incref() {
    if (ref_) {
      ref_->Incref();
    }
  }

  void Incwref() {
    if (ref_) {
      ref_->Incwref();
    }
  }

  void Decref() {
    if (ref_) {
      ref_->Decref();
    }
  }

  void Decwref() {
    if (ref_) {
      ref_->Decwref();
    }
  }

  uint64_t UseCount() { return ref_ ? ref_->UseCount() : 0; }
  uint64_t WeakCount() { return ref_ ? ref_->WeakCount() : 0; }
  element_type* Get() { return ptr_; }

 protected:
  template <typename T2>
  void MoveConstructFrom(PtrBase<T2>&& right) {
    ref_ = right.ref_;
    ptr_ = right.ptr_;

    right.ref_ = nullptr;
    right.ptr_ = nullptr;
  }

  template <typename T2>
  void CopyConstructFrom(const PtrBase<T2>& other) {
    ref_ = other.ref_;
    ptr_ = other.ptr_;

    if (ref_) {
      ref_->Incref();
    }
  }

  template <typename T2>
  void WeaklyConstructFrom(const PtrBase<T2>& other) {
    if (other.ref_) {
      ref_ = other.ref_;
      ptr_ = other.ptr_;
      ref_->Incwref();
    }
  }

  template <class T0>
  friend class WeakPtr;  // specifically, weak_ptr::lock()

  template <class T2>
  bool ConstructFromWeak(const WeakPtr<T2>& other) noexcept {
    // implement shared_ptr's ctor from weak_ptr, and weak_ptr::lock()
    if (other.ptr_) {
      ptr_ = other.ptr_;
      ref_ = other.ref_;
      ref_->Incref();
      return true;
    }

    return false;
  }

 public:
  RefCountBase* ref_ = nullptr;
  element_type* ptr_ = nullptr;
};