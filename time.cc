#include <chrono>
#include <iomanip>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

void test() {
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  cout << ts.tv_sec << "." << ts.tv_nsec << endl;

  timeval tv;
  gettimeofday(&tv, nullptr);
  cout << tv.tv_sec << "." << tv.tv_usec << endl;

  auto now = chrono::system_clock::now();
  cout << now.time_since_epoch().count() << endl;

  auto ms = chrono::time_point_cast<chrono::milliseconds>(now);
  cout << ms.time_since_epoch().count() << endl;

  auto t = chrono::system_clock::to_time_t(ms);
  cout << t << endl;
  cout << put_time(localtime(&t), "%T") << "." << setw(3) << setfill('0')
       << ms.time_since_epoch().count() % 1000 << endl;
}

int main() {
  test();
  sleep(1);
  test();
}
