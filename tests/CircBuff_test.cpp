#include "libs/CircBuff.h"

#include <gtest/gtest.h>

TEST(CircBuffTest, DefaultConstructorTest) {
  CircBuff<int> cb;
  EXPECT_TRUE(cb.empty());
  EXPECT_EQ(cb.size(), 0);
  EXPECT_EQ(cb.capacity(), 0);
}

TEST(CircBuffTest, ConstructorWithCapacityTest) {
  const size_t capacity = 10;
  CircBuff<int> cb(capacity);
  EXPECT_TRUE(cb.empty());
  EXPECT_EQ(cb.size(), 0);
  EXPECT_EQ(cb.capacity(), capacity);
}

TEST(CircBuffTest, ConstructorWithCapacityAndDefaultValueTest) {
  const size_t capacity = 10;
  const int default_value = 5;
  CircBuff<int> cb(capacity, default_value);
  EXPECT_FALSE(cb.empty());
  EXPECT_EQ(cb.size(), capacity);
  EXPECT_EQ(cb.capacity(), capacity);
  for (const auto& el : cb) {
    EXPECT_EQ(el, default_value);
  }
}

TEST(CircBuffTest, InitializerListConstructorTest) {
  CircBuff<int> cb{1, 2, 3, 4, 5};
  EXPECT_FALSE(cb.empty());
  EXPECT_EQ(cb.size(), 5);
  EXPECT_EQ(cb.capacity(), 5);
  int expected = 1;
  for (const auto& el : cb) {
    EXPECT_EQ(el, expected);
    ++expected;
  }
}

TEST(CircBuffTest, CopyConstructorTest) {
  const size_t capacity = 5;
  CircBuff<int> cb1({1, 2, 3, 4, 5});
  CircBuff<int> cb2(cb1);
  EXPECT_EQ(cb1.size(), cb2.size());
  EXPECT_EQ(cb1.capacity(), cb2.capacity());
  EXPECT_EQ(cb1.empty(), cb2.empty());
  auto it1 = cb1.begin();
  auto it2 = cb2.begin();
  while (it1 != cb1.end() && it2 != cb2.end()) {
    EXPECT_EQ(*it1, *it2);
    ++it1;
    ++it2;
  }
}

TEST(CircBufferTest, BeginEndTest) {
  CircBuff<int> cb(5);
  cb.push(1);
  cb.push(2);
  cb.push(3);
  auto it = cb.begin();
  EXPECT_EQ(*it, 1);
  it++;
  EXPECT_EQ(*it, 2);
  it++;
  EXPECT_EQ(*it, 3);
  it++;
  EXPECT_EQ(it, cb.end());
}

TEST(CircBufferTest, CBeginCEndTest) {
  CircBuff<int> cb(5);
  cb.push(1);
  cb.push(2);
  cb.push(3);
  auto cit = cb.cbegin();
  EXPECT_EQ(*cit, 1);
  cit++;
  EXPECT_EQ(*cit, 2);
  cit++;
  EXPECT_EQ(*cit, 3);
  cit++;
  EXPECT_EQ(cit, cb.cend());
}

TEST(CircBufferTest, MaxSizeTest) {
  CircBuff<int> cb(5);
  EXPECT_EQ(cb.max_size(), std::allocator<int>().max_size());
}

TEST(CircBufferTest, AssignmentOperatorTest) {
  CircBuff<int> cb1(5);
  cb1.push(1);
  cb1.push(2);
  cb1.push(3);
  CircBuff<int> cb2(3);
  cb2.push(4);
  cb2.push(5);
  cb2 = cb1;
  EXPECT_EQ(cb2.size(), 3);
  EXPECT_EQ(cb2.capacity(), 5);
  auto it1 = cb1.begin();
  auto it2 = cb2.begin();
  for (; it1 != cb1.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

TEST(CircBuffTest, PopFromEmptyBufferThrowsException) {
  CircBuff<int> buff(2);
  EXPECT_THROW(buff.pop(), std::runtime_error);
}

TEST(CircBuffTest, AddAndRemoveMultipleElements) {
  CircBuff<int> buff(5);
  buff.push(1);
  buff.push(2);
  buff.pop();
  buff.push(3);
  buff.pop();
  buff.push(4);
  EXPECT_EQ(buff.size(), 2);
}

TEST(CircBuffTest, AddAndRemoveMultipleElementsCheckOrder) {
  CircBuff<int> buff(5);
  buff.push(1);
  buff.push(2);
  buff.pop();
  buff.push(3);
  buff.pop();
  buff.push(4);
  EXPECT_EQ(*buff.begin(), 3);
  EXPECT_EQ(*(buff.begin() + 1), 4);
}

TEST(CircBuffTest, CircularPushTest) {
  CircBuff<int> buff(3);
  buff.push(1);
  buff.push(2);
  buff.push(3);
  buff.push(4);
  EXPECT_EQ(*buff.begin(), 2);
  EXPECT_EQ(*(buff.begin() + 2), 4);
}

TEST(CircBuffTest, ReserveTest) {
  CircBuff<int> buf(2);
  buf.push(1);
  buf.push(2);
  buf.reserve(3);
  EXPECT_EQ(buf.capacity(), 3);
  EXPECT_EQ(buf.size(), 2);
  EXPECT_EQ(buf[0], 1);
  EXPECT_EQ(buf[1], 2);
  buf.reserve(1);
  EXPECT_EQ(buf.capacity(), 3);
  EXPECT_EQ(buf.size(), 2);
  EXPECT_EQ(buf[0], 1);
  EXPECT_EQ(buf[1], 2);
}

TEST(CircBuffTest, ResizeTest) {
  CircBuff<int> buf(3);
  buf.push(1);
  buf.push(2);
  buf.push(3);
  buf.resize(2);
  EXPECT_EQ(buf.capacity(), 2);
  EXPECT_EQ(buf.size(), 2);
  EXPECT_EQ(buf[0], 1);
  EXPECT_EQ(buf[1], 2);
  buf.resize(5, 0);
  EXPECT_EQ(buf.capacity(), 5);
  EXPECT_EQ(buf.size(), 5);
  EXPECT_EQ(buf[0], 1);
  EXPECT_EQ(buf[1], 2);
  EXPECT_EQ(buf[2], 0);
  EXPECT_EQ(buf[3], 0);
  EXPECT_EQ(buf[4], 0);
}

TEST(CircBuffTest, InsertValueTest) {
  CircBuff<int> buf(2);
  buf.push(1);
  buf.push(2);
  auto it = buf.insert(buf.begin(), 0);
  EXPECT_EQ(buf.capacity(), 3);
  EXPECT_EQ(buf.size(), 3);
  EXPECT_EQ(buf[0], 0);
  EXPECT_EQ(buf[1], 1);
  EXPECT_EQ(buf[2], 2);
}

TEST(CircBuffTest, InsertMultiValueTest) {
  CircBuff<int> buf(2);
  buf.push(1);
  buf.push(2);
  buf.insert(buf.begin() + 1, 2, 3);
  EXPECT_EQ(buf.capacity(), 4);
  EXPECT_EQ(buf.size(), 4);
  EXPECT_EQ(buf[0], 1);
  EXPECT_EQ(buf[1], 3);
  EXPECT_EQ(buf[2], 3);
  EXPECT_EQ(buf[3], 2);
}
TEST(CircBuffTest, InsertRangeTest) {
  CircBuff<int> buf({1, 2, 3});
  CircBuff<int> cb = {4, 5, 6};
  buf.insert(buf.begin(), cb.begin(), cb.end());
  EXPECT_EQ(buf.capacity(), 6);
  EXPECT_EQ(buf.size(), 6);
  EXPECT_EQ(buf[0], 4);
  EXPECT_EQ(buf[1], 5);
  EXPECT_EQ(buf[2], 6);
  EXPECT_EQ(buf[3], 1);
  EXPECT_EQ(buf[4], 2);
  EXPECT_EQ(buf[5], 3);
}
TEST(CircBuffTest, InsertInitListTest) {
  CircBuff<int> buf({1, 2});
  buf.insert(buf.begin() + 1, {3, 4, 5});
  EXPECT_EQ(buf.capacity(), 5);
  EXPECT_EQ(buf.size(), 5);
  EXPECT_EQ(buf[0], 1);
  EXPECT_EQ(buf[1], 3);
  EXPECT_EQ(buf[2], 4);
  EXPECT_EQ(buf[3], 5);
  EXPECT_EQ(buf[4], 2);
}

TEST(CircBuffTest, EraseSingleElement) {
  CircBuff<int> cb = {1, 2, 3, 4, 5};
  auto it = cb.erase(cb.begin() + 2);
  ASSERT_EQ(*it, 4);
  ASSERT_EQ(cb.size(), 4);
  ASSERT_EQ(cb[0], 1);
  ASSERT_EQ(cb[1], 2);
  ASSERT_EQ(cb[2], 4);
  ASSERT_EQ(cb[3], 5);
}

TEST(CircBuffTest, EraseRange) {
  CircBuff<int> cb = {1, 2, 3, 4, 5};
  auto it = cb.erase(cb.begin() + 1, cb.begin() + 4);
  ASSERT_EQ(*it, 5);
  ASSERT_EQ(cb.size(), 2);
  ASSERT_EQ(cb[0], 1);
  ASSERT_EQ(cb[1], 5);
}
TEST(CircBuffTest, AssignRange) {
  CircBuff<int> cb = {1, 2, 3, 4, 5};
  CircBuff<int> cb2(3);
  cb2.assign(cb.begin() + 1, cb.begin() + 4);
  ASSERT_EQ(cb2.size(), 3);
  ASSERT_EQ(cb2[0], 2);
  ASSERT_EQ(cb2[1], 3);
  ASSERT_EQ(cb2[2], 4);
}

TEST(CircBuffTest, AssignInitializerList) {
  CircBuff<int> cb = {1, 2, 3, 4, 5};
  cb.assign({6, 7, 8});
  ASSERT_EQ(cb.size(), 3);
  ASSERT_EQ(cb[0], 6);
  ASSERT_EQ(cb[1], 7);
  ASSERT_EQ(cb[2], 8);
}

TEST(CircBuffTest, AssignDefaultValue) {
  CircBuff<int> cb = {1, 2, 3, 4, 5};
  cb.assign(3, 9);
  ASSERT_EQ(cb.size(), 3);
  ASSERT_EQ(cb[0], 9);
  ASSERT_EQ(cb[1], 9);
  ASSERT_EQ(cb[2], 9);
}

TEST(CircBuffTest, Clear) {
  CircBuff<int> cb = {1, 2, 3, 4, 5};
  cb.clear();
  ASSERT_EQ(cb.size(), 0);
}

TEST(CircBuffTest, Swap) {
  CircBuff<int> cb1(5);
  CircBuff<int> cb2(10);
  for (int i = 1; i <= 5; ++i) {
    cb1.push(i);
  }
  for (int i = 1; i <= 10; ++i) {
    cb2.push(i);
  }
  CircBuff<int> cb1_copy = cb1;
  CircBuff<int> cb2_copy = cb2;
  cb1.swap(cb2);
  EXPECT_EQ(cb1.size(), 10);
  EXPECT_EQ(cb2.size(), 5);
  for (int i = 1; i <= 10; ++i) {
    EXPECT_EQ(cb1[i-1], i);
  }
  for (int i = 1; i <= 5; ++i) {
    EXPECT_EQ(cb2[i-1], i);
  }
  swap(cb1_copy, cb2_copy);
  EXPECT_EQ(cb1_copy.size(), 10);
  EXPECT_EQ(cb2_copy.size(), 5);
  for (int i = 1; i <= 10; ++i) {
    EXPECT_EQ(cb1_copy[i-1], i);
  }
  for (int i = 1; i <= 5; ++i) {
    EXPECT_EQ(cb2_copy[i-1], i);
  }
}

TEST(CircBuffTest, FindCompatibility) {
  CircBuff<int> buff(5);
  buff.push(1);
  buff.push(2);
  buff.push(3);

  auto result = std::find(buff.begin(), buff.end(), 2);

  EXPECT_EQ(*result, 2);
}

TEST(CircBuffTest, SortCompatibility) {
  CircBuff<int> buff(5);
  buff.push(3);
  buff.push(1);
  buff.push(2);

  std::sort(std::begin(buff), std::end(buff));

  EXPECT_EQ(buff[0], 1);
  EXPECT_EQ(buff[1], 2);
  EXPECT_EQ(buff[2], 3);
}

TEST(CircBuffTest, ForEachCompatibility) {
  CircBuff<int> buff(5);
  buff.push(1);
  buff.push(2);
  buff.push(3);

  int sum = 0;
  std::for_each(std::begin(buff), std::end(buff), [&](int value) {
    sum += value;
  });

  EXPECT_EQ(sum, 6);
}

// Тестирование совместимости с std::count
TEST(CircBuffTest, CountCompatibility) {
  CircBuff<int> buff(5);
  buff.push(1);
  buff.push(2);
  buff.push(2);

  // Считаем количество вхождений значения 2 с использованием std::count
  int count = std::count(std::begin(buff), std::end(buff), 2);

  // Проверяем, что значение 2 встречается 2 раза
  EXPECT_EQ(count, 2);
}
