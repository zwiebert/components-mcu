#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <utils_misc/ftoa.h>

void tst_ftoa() {
  char buf[32];
  const char *s = ftoa(53.12, buf, 2);
  TEST_ASSERT_EQUAL_STRING("53.12", s);
}

TEST_CASE("ftoa", "[utils_misc]")
{
  tst_ftoa();
}
