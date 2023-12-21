#pragma once

#include <gtest/gtest.h>

struct Test : testing::Test {
  static void SetUpTestSuite();
  void TearDown() override;
  void test();
};