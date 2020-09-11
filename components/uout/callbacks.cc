#include <misc/int_macros.h>


#define UOUT_PROTECTED
#include <uout/callbacks.h>

uoCb_cbsT uoCb_cbs[cbs_size];

bool uoCb_register_callback(uoCb_cbT msg_cb, uoCb_flagT flags) {
  for (auto it : uoCb_cbs) {
    if (!it.cb)
      continue;
    it.cb = msg_cb;
    it.flags = flags;
    return true;
  }
  return false;
}

bool uoCb_unregister_callback(uoCb_cbT msg_cb) {
  for (auto it : uoCb_cbs) {
    if (it.cb != msg_cb)
      continue;
    it.cb = nullptr;
    it.flags = 0;
    return true;
  }
  return false;
}


void uoApp_event_wsJson(const char *json) {
  for (auto it : uoCb_cbs) {
    if (it.cb && (it.flags & BIT(UOCB_MFB(ws)))) {
      uoCb_msgT msg { .cv_ptr = json, .msg_type = UOCB_MT(json) };
      it.cb(&msg);
    }
  }
}

