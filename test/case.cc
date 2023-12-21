#include "repo.h"
#include "test.h"
#include "wang.h"

TEST_F(Test, one) {
  EXPECT_CALL(get<Wang>("")->mock(), call(testing::StrEq("test")))
      .WillOnce(testing::Invoke([] { std::cout << __func__ << std::endl; }));
  test();
}

TEST_F(Test, two) {
  EXPECT_CALL(get<Wang>("")->mock(), call).Times(testing::AnyNumber());
  test();
}
