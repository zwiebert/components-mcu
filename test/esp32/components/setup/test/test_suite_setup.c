#include "unity.h"

#include "key_value_store/kvs_wrapper.h"
#include "storage/storage.h"
#include "txtio/inout.h"
#include "cli/mutex.h"

int unit_testing;

extern "C++" void unity_suiteSetUp() {

  unit_testing = 1;

  mutex_setup();
  kvs_setup();
  txtio_setup();
  stor_setup();
}
