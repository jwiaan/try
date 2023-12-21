#include "test.h"
#include "repo.h"
#include "wang.h"

void Test::SetUpTestSuite() { add("", Wang::create()); }
void Test::TearDown() { Repo::repo().reset(); }
void Test::test() {
  while (1) {
    char s[4096];
    auto n = read(STDIN_FILENO, s, sizeof(s));
    if (!n)
      return;

    s[n - 1] = 0;
    get<Wang>("")->call(s);
  }
}
