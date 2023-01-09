#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif


#include "../include/config_kvs/config.h"
#include "../config_kvs.h"
#include <config_kvs/comp_settings.hh>
#include "key_value_store/kvs_wrapper.h"

#include "utils_misc/int_types.h"
#include "utils_misc/itoa.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <utils_misc/cstring_utils.hh>

static void tst_compSettings() {

  config_save_item_s("key1", "val1");
  {
    char buf[80] = "---------------";
    config_read_item_s("key1", buf, sizeof buf, "def1");
    TEST_ASSERT_EQUAL_STRING(buf, "val1");
  }

  {
    char buf[80] = "---------------";
    TEST_ASSERT_EQUAL_STRING("C_VERBOSE", settings_get_kvsKey(CB_VERBOSE));
    config_save_item_s(settings_get_kvsKey(CB_VERBOSE), "val2");
    config_read_item_s(settings_get_kvsKey(CB_VERBOSE), buf, sizeof buf, "def2");
    TEST_ASSERT_EQUAL_STRING(buf, "val2");
  }

  {
    char buf[80] = "---------------";
    config_save_item_s(settings_get_kvsKey(CB_VERBOSE), "val3");
    kvshT h;
    if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
      kvsRead_charArray(h, CB_VERBOSE, buf);
      kvs_close(h);
    }
    TEST_ASSERT_EQUAL_STRING(buf, "val3");
  }
}

TEST_CASE("CompSettings", "[config]")
{
  tst_compSettings();
}
