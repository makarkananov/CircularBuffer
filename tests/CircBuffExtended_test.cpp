#include "libs/CircBuff.h"

#include <gtest/gtest.h>

TEST(CircBuffExtendedTest, ExtendedPushTest) {
  CircBuffExtended<int> buff(3);
  buff.push(1);
  buff.push(2);
  buff.push(3);
  buff.push(4);
  EXPECT_EQ(buff.capacity(), 6);
  EXPECT_EQ(buff.size(), 4);
  EXPECT_EQ(*buff.begin(), 1);
  EXPECT_EQ(*(buff.begin() + 3), 4);
}