#include "SharedPtr.h"

int main() {
  CombinPair<RefCountBase, int> pair;
  pair.InitFirst();
  pair.InitSecond();

  pair.First()->Incwref();
  *(pair.Second()) = 5;
  return 0;
}