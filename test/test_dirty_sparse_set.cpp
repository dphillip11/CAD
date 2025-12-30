#include <cassert>
#include <iostream>

#include "Utilities/SparseSet.h"

int main() {
  bool dirty = false;
  DirtySparseSet<int> set(dirty);

  // Initially not dirty
  assert(!dirty);

  // Emplace should set dirty
  Id id1 = set.Emplace(42);
  assert(dirty);
  dirty = false;  // reset

  // Insert should set dirty
  Id id2 = set.Insert(24);
  assert(dirty);
  dirty = false;

  // Remove should set dirty
  set.Remove(id1);
  assert(dirty);
  dirty = false;

  // Non-mutating operations should not set dirty
  int val = set.Get(id2);
  assert(val == 24);
  assert(!dirty);

  bool contains = set.Contains(id2);
  assert(contains);
  assert(!dirty);

  std::cout << "All tests passed!" << std::endl;
  return 0;
}