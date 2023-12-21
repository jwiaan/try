#include "wang.h"

namespace {
class Impl : public Wang {
public:
  Impl() { reset(); }
  void call(const char *s) override { m_->call(s); }
  Mock &mock() override { return *m_; }
  void reset() override {
    m_ = std::make_unique<Mock>();
    ON_CALL(*m_, call).WillByDefault(testing::Invoke(this, &Impl::print));
  }

private:
  void print(const char *s) { std::cout << __func__ << s << std::endl; }
  std::unique_ptr<Mock> m_;
};
} // namespace

std::shared_ptr<Wang> Wang::create() { return std::make_shared<Impl>(); }
