#include <iostream>
#include <map>
#include <memory>

using namespace std;

struct Step {
  virtual ~Step() = default;
  virtual size_t run() = 0;
};

struct Start : Step {
  size_t run() override {
    cout << __PRETTY_FUNCTION__ << endl;
    return 0;
  }
};

struct Stop : Step {
  size_t run() override {
    cout << __PRETTY_FUNCTION__ << endl;
    return 0;
  }
};

class Task {
public:
  template <typename... T> void set() {
    size_t k = 0;
    steps = {{++k, make_shared<T>()}...};
  }

  void run() {
    for (auto it = steps.begin(); it != steps.end();) {
      auto k = it->second->run();
      if (k)
        it = steps.find(k);
      else
        ++it;
    }
  }

private:
  map<size_t, shared_ptr<Step>> steps;
};

int main() {
  Task t;
  t.set<Start, Stop>();
  t.run();
}
