#include "libs/CircBuff.h"

#include <algorithm>
#include <iostream>

template<typename T>
void print_buffer(CircBuff<T>& buff) {
  std::cout << "size: " << buff.size() << std::endl;
  std::cout << "begin: " << *(buff.begin()) << std::endl;
  std::cout << "end: " << *(buff.end()) << std::endl;
  for (const auto& item : buff) {
    std::cout << item << " ";
  }
  std::cout << std::endl;
}

int main() {
  CircBuff<int> buf({1, 2});
  buf.insert(buf.begin() + 1, {3, 4, 5});
//  std::sort(buf.begin(), buf.end());
  print_buffer(buf);
  return 0;
}