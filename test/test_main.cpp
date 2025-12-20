#include "hello.h"
#include <gtest/gtest.h>

TEST(HelloTest, ReturnsExpectedMessage) {
  EXPECT_EQ(hello::get_message(), "Hello, World! from CAD");
}
