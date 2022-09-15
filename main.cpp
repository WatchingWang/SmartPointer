#include <iostream>
#include <memory>

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
    std::cout << base->ref_->UseCount();
}

int main() {
  UniquePtr<int, TestDel<int>> b(new int(3));

  SharedPtr<int> a(new int(3));
  std::cout << *a;

  SharedPtr<int> c(std::move(a));
  std::cout << *c;

  WeakPtr<int> d(c);
  d.IsExpired();
  auto e = d.Lock();
  *e = 5;
  std::cout << *e;
  return 0;
}