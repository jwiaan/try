#include <iostream>

void print(std::ostream &s) { s << std::endl; }

template <typename T, typename... U>
void print(std::ostream &s, const T &t, const U &...u) {
  s << t;
  print(s, u...);
}

#define PRINT(...)                                                             \
  do {                                                                         \
    std::cout << __LINE__ << ": ";                                             \
    ::print(std::cout, ##__VA_ARGS__);                                         \
  } while (0)
