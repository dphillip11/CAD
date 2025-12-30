#include <gtest/gtest.h>

#include "Utilities/SparseSet.h"

TEST(DirtySparseSetTest, InitiallyNotDirty) {
  bool dirty = false;
  DirtySparseSet<int> set(dirty);
  EXPECT_FALSE(dirty);
}

TEST(DirtySparseSetTest, EmplaceSetsFlag) {
  bool dirty = false;
  DirtySparseSet<int> set(dirty);

  Id id = set.Emplace(42);
  EXPECT_TRUE(dirty);
  EXPECT_EQ(set.Get(id), 42);
}

TEST(DirtySparseSetTest, InsertSetsFlag) {
  bool dirty = false;
  DirtySparseSet<int> set(dirty);

  Id id = set.Insert(24);
  EXPECT_TRUE(dirty);
  EXPECT_EQ(set.Get(id), 24);
}

TEST(DirtySparseSetTest, RemoveSetsFlag) {
  bool dirty = false;
  DirtySparseSet<int> set(dirty);

  Id id = set.Emplace(42);
  dirty = false;  // reset

  set.Remove(id);
  EXPECT_TRUE(dirty);
}

TEST(DirtySparseSetTest, NonMutatingOperationsDoNotSetFlag) {
  bool dirty = false;
  DirtySparseSet<int> set(dirty);

  Id id = set.Insert(24);
  dirty = false;  // reset

  // Get should not set dirty
  int val = set.Get(id);
  EXPECT_EQ(val, 24);
  EXPECT_FALSE(dirty);

  // Contains should not set dirty
  bool contains = set.Contains(id);
  EXPECT_TRUE(contains);
  EXPECT_FALSE(dirty);
}
