#include "repo.h"
#include "stub.h"
#include <map>

namespace {
class Impl : public Repo {
public:
  void reset() override {
    for (const auto &a : m_) {
      for (const auto &b : a.second)
        b.second->reset();
    }
  }

  void add(const std::type_index &i, const std::string &s,
           const std::shared_ptr<Stub> &p) override {
    m_[i][s] = p;
  }

  std::shared_ptr<Stub> get(const std::type_index &i,
                            const std::string &s) override {
    return m_.at(i).at(s);
  }

private:
  std::map<std::type_index, std::map<std::string, std::shared_ptr<Stub>>> m_;
};
} // namespace

Repo &Repo::repo() {
  static Impl i;
  return i;
}
