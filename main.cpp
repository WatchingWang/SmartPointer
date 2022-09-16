#include <atomic>
#include <iostream>
#include <memory>
#include <thread>

#include "SharedPtr.h"
#include "UniquePtr.h"
#include "WeakPtr.h"

template <typename T>
struct TestDel {
  void operator()(T* p) {
    delete p;
    std::cout << "success to delete" << std::endl;
  }
};

template <typename T>
void printRefCount(PtrBase<T>& base) {
  std::cout << "use count: " << base.ref_->UseCount()
            << " weak count: " << base.ref_->WeakCount() << std::endl;
}

static std::atomic_int64_t g_count{0};
class obj : public std::enable_shared_from_this<obj> {
 public:
  void TestOtherThread() {
    g_count.exchange(0);
    auto weak_this = weak_from_this();
    std::thread thd([weak_this] {
      for (int i = 0; i < 100; i++) {
        auto shared_this = weak_this.lock();
        if (shared_this) {
          ++g_count;
          // callback...
        }
      }
    });
    thd.detach();
  }
};

void TestEnableSharedFromThis() {
  {
    auto t = std::make_shared<obj>();
    t->TestOtherThread();
  }

  std::cout << g_count;
}

int main() {
  UniquePtr<int, TestDel<int>> u_a(new int(3));
  UniquePtr<int, TestDel<int>> u_b(std::move(u_a));
  u_a.Reset();

  SharedPtr<int> s_a(new int(3));
  std::cout << *s_a << std::endl;

  SharedPtr<int> s_b(std::move(s_a));
  printRefCount(s_b);

  WeakPtr<int> w_b(s_b);
  printRefCount(s_b);
  if (!w_b.IsExpired()) {
    auto s_c = w_b.Lock();
    printRefCount(s_c);
  }

  //  TestEnableSharedFromThis();
  return 0;
}