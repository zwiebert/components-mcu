#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <jsmn/jsmn_iterate.hh>
char json[] = R"({"json":{"auto":{"adapter.0":{"name":"Neutral","flags":{"exists":1,"neutral":1,"read_only":1},"temp":0.06,"wind":0.001,"humi":0.01,"clouds":0.01}}}})";
char json3[] = R"({"json":{}})";

bool test_td() {
  auto jsmn = Jsmn<64, char *>(json);
  TEST_ASSERT_FALSE(!jsmn);
  auto it = jsmn.begin();

++it;
TEST_ASSERT_TRUE(it.keyStartsWith("jso"));
TEST_ASSERT_FALSE(it.keyStartsWith("gso"));
TEST_ASSERT_FALSE(!it);
it.skip_key_and_value();
TEST_ASSERT_TRUE(!it);
return true;

}




TEST_CASE("crash", "[jsmn]")
{
  test_td();
}
