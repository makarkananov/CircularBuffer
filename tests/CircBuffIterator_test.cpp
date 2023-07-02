#include "libs/CircBuff.h"

#include <gtest/gtest.h>

TEST(CircBuffIteratorTest, IncrementOperatorTest) {
  CircBuff<int> buffer(3);
  buffer.push(1);
  buffer.push(2);
  buffer.push(3);
  auto it = buffer.begin();
  EXPECT_EQ(*it, 1);
  it++;
  EXPECT_EQ(*it, 2);
  it++;
  EXPECT_EQ(*it, 3);
  it++;
  EXPECT_EQ(*it, 1);
}

TEST(CircBuffIteratorTest, OperatorStar) {
  CircBuff<int> buffer{1, 2, 3};
  auto it = buffer.begin() + 1;
  EXPECT_EQ(*it, 2);
}

TEST(CircBuffIteratorTest, OperatorArrow) {
  struct TestStruct {
    int x;
    int y;
  };
  TestStruct obj1 = {0, 100};
  TestStruct obj2 = {100, 1};
  CircBuff<TestStruct> buffer({obj1, obj2});
  auto it = buffer.begin();
  EXPECT_EQ(it->x, 0);
  EXPECT_EQ(it->y, 100);
}

TEST(CircBuffIteratorTest, OperatorBrackets) {
  CircBuff<int> buff({1, 2, 3});
  auto it = buff.begin() + 1;
  EXPECT_EQ(it[1], 3);
  EXPECT_EQ(it[2], 1);
}

TEST(CircBuffIteratorTest, ComparisonOperators) {
  CircBuff<int> buff({1, 2, 3});
  auto it1 = buff.begin();
  auto it2 = it1 + 1;
  auto it3 = it2 + 1;
  EXPECT_TRUE(it1 < it2);
  EXPECT_FALSE(it1 > it2);
  EXPECT_TRUE(it2 > it1);
  EXPECT_FALSE(it2 < it1);
  EXPECT_TRUE(it1 <= it2);
  EXPECT_FALSE(it1 >= it2);
  EXPECT_TRUE(it2 >= it1);
  EXPECT_FALSE(it2 <= it1);
  EXPECT_FALSE(it1 == it3);
  EXPECT_TRUE(it1 != it3);
  EXPECT_FALSE(it1 == it2);
  EXPECT_TRUE(it1 != it2);
}