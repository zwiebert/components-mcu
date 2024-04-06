
#include "uout/so_out.h"
#include <uout/uout_writer.hh>
#include "uout/uout_builder_json.hh"
#include "debug/dbg.h"
#include "utils_misc/int_macros.h"
#include <string.h>
#include <stdio.h>


#ifdef UOUT_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "uout.so_out"

/////////////////////////Public////////////////////////////////
bool UoutPrinter::hasOutput_text() const {
  return myTd.tgt() & SO_TGT_FLAG_TXT;
}
bool UoutPrinter::hasOutput_json() const {
  return myTd.tgt() & SO_TGT_FLAG_JSON;
}

void UoutPrinter::x_open(const char *name) const {
  if (hasOutput_text())
    myTd.st().cli_out_set_x(name);
  if (hasOutput_json()) {
    myTd.sj().add_object(name);
  }
}

void UoutPrinter::x_close() const {
  if (hasOutput_text())
    myTd.st().cli_out_close();
  if (hasOutput_json()) {
    myTd.sj().close_object();

  }
}

bool UoutPrinter::root_open(const char *name) const {
  bool error = false;
  if (hasOutput_json() && !myTd.sj().open_root_object(name))
    error = true;

  return !error;
}

void UoutPrinter::root_close() const {
  if (hasOutput_text())
    myTd.st().cli_out_close();
  if (hasOutput_json()) {
    myTd.sj().close_root_object();
  }
}

void UoutPrinter::print(const char *key, const char *val) const {
  if (hasOutput_text())
    myTd.st().cli_out_x_reply_entry(key, val);
  if (hasOutput_json())
    myTd.sj().add_key_value_pair_s(key, val);
}
void UoutPrinter::print(const char *key, int val) const {
  char buf[20];
  snprintf(buf, sizeof buf, "%d", val);
  if (hasOutput_text())
    myTd.st().cli_out_x_reply_entry(key, buf);
  if (hasOutput_json())
    myTd.sj().add_key_value_pair_d(key, val);
}

void UoutPrinter::print(const char *key, long val) const {
  char buf[20];
  snprintf(buf, sizeof buf, "%ld", val);
  if (hasOutput_text())
    myTd.st().cli_out_x_reply_entry(key, buf);
  if (hasOutput_json())
    myTd.sj().add_key_value_pair_d(key, val);
}

void UoutPrinter::print(const char *key, long long val) const {
  char buf[20];
  snprintf(buf, sizeof buf, "%lld", val);
  if (hasOutput_text())
    myTd.st().cli_out_x_reply_entry(key, buf);
  if (hasOutput_json())
    myTd.sj().add_key_value_pair_d(key, val);
}

void UoutPrinter::print(const char *key, unsigned val, int base) const {
  char buf[20];
  snprintf(buf, sizeof buf, base == 10 ? "%u" : "%x", val);
  if (hasOutput_text())
    myTd.st().cli_out_x_reply_entry(key, buf);
  if (hasOutput_json()) {
    if (base == 10)
      myTd.sj().add_key_value_pair_d(key, val);
    else
      myTd.sj().add_key_value_pair_s(key, buf); //no hex in json. use string
  }
}

void UoutPrinter::print(const char *key, unsigned long val, int base) const {
  char buf[20];
  snprintf(buf, sizeof buf, base == 10 ? "%lu" : "%lx", val);
  if (hasOutput_text())
    myTd.st().cli_out_x_reply_entry(key, buf);
  if (hasOutput_json()) {
    if (base == 10)
      myTd.sj().add_key_value_pair_d(key, val);
    else
      myTd.sj().add_key_value_pair_s(key, buf); //no hex in json. use string
  }
}

void UoutPrinter::print(const char *key, float val, int n) const {
  precond(0 <= n && n <= 9);
  char buf[20];
  char fmt[] = "%.2f";
  fmt[2] = '0' + n;
  snprintf(buf, sizeof buf, fmt, val); //
  if (hasOutput_text())
    myTd.st().cli_out_x_reply_entry(key, buf);
  if (hasOutput_json())
    myTd.sj().add_key_value_pair_f(key, val, n);
}

