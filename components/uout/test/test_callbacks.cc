#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <uout/uo_callbacks.h>

so_arg_pch_t Pch;
char MsgTxt[80];
char MsgJson[80];

static void cb_any(const uoCb_msgT msg) {
  if (auto s = uoCb_txtFromMsg(msg); s) {
    strcpy(MsgTxt, s);
  }

  if (auto s = uoCb_jsonFromMsg(msg); s) {
    strcpy(MsgJson, s);
  }
}

static void cb_pch(const uoCb_msgT msg) {
  if (msg.flags.fmt.raw) {
    Pch = *static_cast<const so_arg_pch_t*>(msg.cptr);
  }
}

static void subscribe_any() {
  uoCb_unsubscribe(cb_any);

  uo_flagsT flags;
  flags.evt_flags = 0xff;
  flags.tgt_flags = 0xff;
  flags.fmt.json = true;
  flags.fmt.txt = true;
  uoCb_subscribe(cb_any, flags);
}

static void subscribe_any_flags(uo_flagsT flags) {
  uoCb_unsubscribe(cb_any);

  uoCb_subscribe(cb_any, flags);
}


static void subscribe_pch() {
  uoCb_unsubscribe(cb_pch);
  uo_flagsT flags;
  flags.evt.pin_change = true;
  flags.fmt.raw = true;
  uoCb_subscribe(cb_pch, flags);
}

static void t_pch() {
  subscribe_pch();
  uo_flagsT flags;
  flags.evt.pin_change = true;
  flags.fmt.json = true;
  subscribe_any_flags(flags);

  uoCb_publish_pinChange( { 4, true });
  TEST_ASSERT_EQUAL(4, Pch.gpio_num);
  TEST_ASSERT_TRUE(Pch.level);

  //TEST_ASSERT_EQUAL_STRING("", MsgTxt);
  TEST_ASSERT_EQUAL_STRING("{\"mcu\":{\"gpio4\":1}}", MsgJson);
}

static void t_ipAddr() {
  uo_flagsT flags;
  flags.evt.ip_address_change = true;
  flags.fmt.json = true;
  flags.fmt.txt = true;
  subscribe_any_flags(flags);

  uoCb_publish_ipAddress("192.8.9.10");

  TEST_ASSERT_EQUAL_STRING("tf: ipaddr: 192.8.9.10;\n", MsgTxt);
  TEST_ASSERT_EQUAL_STRING("{\"mcu\":{\"ipaddr\":\"192.8.9.10\"}}", MsgJson);
}

static void t_wsJson() {
  uo_flagsT flags;
  flags.tgt.websocket = true;
  flags.fmt.json = true;
  subscribe_any_flags(flags);
  const char *json = "{\"abc\":[1,2,3]}}";

  uoCb_publish_wsJson(json);

  TEST_ASSERT_EQUAL_STRING(json, MsgJson);
}

TEST_CASE("callbacks", "[uout]")
{
  t_pch();
  t_ipAddr();
  t_wsJson();
}
