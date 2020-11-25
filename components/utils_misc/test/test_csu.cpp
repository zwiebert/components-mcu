#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <utils_misc/cstring_utils.h>

void tst_csuEndsWith() {
  TEST_ASSERT_TRUE(csu_endsWith("net?","?"));
}

TEST_CASE("endsWith", "[utils_misc]")
{
  tst_csuEndsWith();
}
