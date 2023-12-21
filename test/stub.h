#pragma once

struct Stub {
  virtual ~Stub() = default;
  virtual void reset() = 0;
};