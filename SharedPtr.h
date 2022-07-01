#include <atomic>

template <typename T1, typename T2>
class CombinPair {
 public:
  CombinPair() { mem_ = malloc(sizeof(T1) + sizeof(T2)); }

  T1* First() { return static_cast<T1>(mem_); }
  T2* Second() { return static_cast<T2>(mem_ + sizeof(T1)); }

  template <typename... Args>
  T1* InitFirst(Args&&... args) {
    ::new (First)(std::forward<Args>(args)...);
  }

  template <typename... Args>
  T1* InitSecond(Args&&... args) {
    ::new (Second)(std::forward<Args>(args)...);
  }

 private:
  void* mem_;
};

class RefCountBase {
 public:
  void Incref();

 private:
  std::atomic_uint64_t weaks_;
  std::atomic_uint64_t uses_;
};

template <typename T, typename DEL>
class SharedPtr {
 public:
};