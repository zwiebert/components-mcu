#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif
#include "key_value_store/kvs_wrapper.h"
#include <string.h>

kvshT handle;
bool succ;
int res;
const char *NS = "testing";
const char *asdfg = "asdfg", *asdf = "asdf", *asdfge = "asdfge";

struct b { char data[8]; };


static void test_for_foreach_bug() {
  struct b b = { .data = { 0, 1, 2, 3, 4, 5, 6, 7 } };
  const char *k1 = "blob_1", *k2 = "blob_2", *k3 = "blob_3";

  //---------------------------
  handle = kvs_open(NS, kvs_WRITE);
  TEST_ASSERT_NOT_NULL(handle);

  succ = kvs_rw_blob(handle, k1, &b, sizeof b, true);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);
  kvs_close(handle);

  return;

  res = kvs_foreach(NS, KVS_TYPE_BLOB, k1, 0, 0);
  TEST_ASSERT_EQUAL(1, res);
  //-------------------------------
  handle = kvs_open(NS, kvs_WRITE);
  TEST_ASSERT_NOT_NULL(handle);

  succ = kvs_rw_blob(handle, k1, &b, sizeof b, true);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);
  kvs_close(handle);

  res = kvs_foreach(NS, KVS_TYPE_BLOB, k1, 0, 0);
  TEST_ASSERT_EQUAL(1, res);
  //--------------------------------
}

static void test_config() {
  char buf[128];
#define test_mqtt_user "zimbra"
  handle = kvs_open(NS, kvs_WRITE);
  TEST_ASSERT_NOT_NULL(handle);
  succ = kvs_rw_str(handle, "C_MQTT_CID", "tfmcu_esp8266", 0, true);
  TEST_ASSERT_TRUE(succ);
  kvs_close(handle);

  handle = kvs_open(NS, kvs_READ);
  TEST_ASSERT_NOT_NULL(handle);
  strcpy(buf, test_mqtt_user);
  succ = kvs_rw_str(handle, "C_MQTT_USER", buf, 128, false);
  TEST_ASSERT_FALSE(succ);
  TEST_ASSERT_EQUAL_STRING(test_mqtt_user, buf);  // original string must be untouched
  kvs_close(handle);
}

static void test_set_get_default() {
  handle = kvs_open(NS, kvs_READ_WRITE);
  TEST_ASSERT_NOT_NULL(handle);
  succ = kvs_erase_key(handle, asdf);
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);

  int8_t res = kvs_get_i8(handle, asdf, 3, &succ);
  TEST_ASSERT_FALSE(succ);
  TEST_ASSERT_EQUAL(3, res);

  succ = kvs_set_i8(handle, asdf, 42);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);

  res = kvs_get_i8(handle, asdf, 3, &succ);
  TEST_ASSERT_TRUE(succ);
  TEST_ASSERT_EQUAL(42, res);

  kvs_close(handle);
}

static void g(uint8_t g, uint8_t m) {
  handle = kvs_open(NS, kvs_WRITE);
  TEST_ASSERT_NOT_NULL(handle);
  succ = kvs_set_i8(handle, asdfg, 42);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);
  kvs_close(handle);

  handle = kvs_open(NS, kvs_READ);
  TEST_ASSERT_NOT_NULL(handle);
  succ = kvs_set_i8(handle, asdf, 42);
  TEST_ASSERT_FALSE_MESSAGE(succ, "handle is read-only");
  kvs_close(handle);

  handle = kvs_open(NS, kvs_WRITE);
  TEST_ASSERT_NOT_NULL(handle);
  succ = kvs_set_i8(handle, asdf, 42);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);
  kvs_close(handle);

  res = kvs_foreach(NS, KVS_TYPE_i8, asdf, 0, 0);
  TEST_ASSERT_EQUAL(2, res);
  res = kvs_foreach(NS, KVS_TYPE_i8, asdfg, 0, 0);
  TEST_ASSERT_EQUAL(1, res);
  res = kvs_foreach(NS, KVS_TYPE_u8, asdfg, 0, 0);
  TEST_ASSERT_EQUAL_MESSAGE(0, res, "wrong type");
  res = kvs_foreach(NS, KVS_TYPE_i8, asdfge, 0, 0);
  TEST_ASSERT_EQUAL_MESSAGE(0, res, "match string longer than key");

  handle = kvs_open(NS, kvs_WRITE);
  TEST_ASSERT_NOT_NULL(handle);
  succ = kvs_set_i8(handle, asdf, 42);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);
  kvs_close(handle);

  res = kvs_foreach(NS, KVS_TYPE_i8, asdf, 0, 0);
  TEST_ASSERT_EQUAL(2, res);

  handle = kvs_open(NS, kvs_WRITE);
  succ = kvs_erase_key(handle, asdf);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_erase_key(handle, asdfg);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_erase_key(handle, asdfge);
  TEST_ASSERT_FALSE_MESSAGE(succ, "non-existing key");
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);
  kvs_close(handle);
}

static void f() {
  g(1,2);
}

TEST_CASE("kvs", "[kvs]") {
  test_config();
  test_set_get_default();
  f();
  test_for_foreach_bug();
  test_set_get_default();
}
