#pragma once

#include "stub.h"
#include <gmock/gmock.h>

struct Mock {
  MOCK_METHOD(void, call, (const char *));
};

struct Wang : Stub {
  static std::shared_ptr<Wang> create();
  virtual void call(const char *) = 0;
  virtual Mock &mock() = 0;
};