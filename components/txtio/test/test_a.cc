#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif
#include "../include/txtio/inout.h"

char buf[512] = "42";


TEST_CASE("test name", "[txtio]")
{
        TEST_ASSERT_EQUAL_STRING("42", buf);
        //TEST_ASSERT_TRUE(0);
}
