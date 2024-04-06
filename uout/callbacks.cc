#include <utils_misc/int_macros.h>
#include <utils_misc/mutex.hh>
#include <uout/uo_callbacks.h>
#include "debug/dbg.h"

#include <stdio.h>

static uoCb_cbsT uoCb_cbs[cbs_size]; ///< store call-back pointers
static bool uoCb_cbs_enable[cbs_size]; ///< enable/disable call backs (to avoid shifting them around)

using OurMutex = RecMutex;
using OurLockGuard = std::lock_guard<OurMutex>;

/*
 * \brief protect while modifying callback arrays uoCb_cbs, uoCb_cbs_enable
 *        To prevent deadlock: Do not* lock when calling external functions which may run into other mutex/semaphore
 *
 */
static OurMutex uoCb_mutex;

/////////////////// Filter /////////////////////////////////////
static uoCb_Idxs get_all_callbacks() {
  OurLockGuard lock(uoCb_mutex);

  auto result = uoCb_Idxs();

  for (int i = 0; i < cbs_size; ++i) {
    if (!uoCb_cbs_enable[i])
      continue;

    result.arr[result.size++] = i;
  }

  return result;
}

// public

uoCb_Idxs uoCb_filter(uo_flagsT flags, uoCb_Idxs idxs) {
  OurLockGuard lock(uoCb_mutex);
  uoCb_Idxs result { };

  for (auto i = 0; i < idxs.size; ++i) {
    auto &it = uoCb_cbs[idxs.arr[i]];
    if (0 == (it.flags.evt_flags & flags.evt_flags) && 0 == (it.flags.tgt_flags & flags.tgt_flags))
      continue;
    if (0 == (it.flags.fmt_flags & flags.fmt_flags))
      continue;

    result.arr[result.size++] = &it - &uoCb_cbs[0];
  }
  return result;
}

uoCb_Idxs uoCb_filter(uo_flagsT flags) {
  return uoCb_filter(flags, get_all_callbacks());
}

/////////////////////// Subscriptions /////////////////////////////

// public

bool uoCb_subscribe(uoCb_cbT msg_cb, uo_flagsT flags) {
  OurLockGuard lock(uoCb_mutex);
  precond(msg_cb);

  // pass-0: try to update or re-enable previous subscribed callback
  // pass-1: register in empty slot preserving any un-subscribed callback
  // pass-2: register in disabled slot
  for (int pass = 0; pass < 3; ++pass) {
    for (int i = 0; i < cbs_size; ++i) {
      if (pass == 0 && uoCb_cbs[i].cb != msg_cb)
        continue;
      if (pass == 1 && uoCb_cbs[i].cb)
        continue;
      if (pass == 2 && uoCb_cbs_enable[i])
        continue;

      uoCb_cbs[i].cb = msg_cb;
      uoCb_cbs[i].flags = flags;
      uoCb_cbs_enable[i] = true;
      return true;

    }
  }

  return false;

}

bool uoCb_unsubscribe(uoCb_cbT msg_cb) {
  OurLockGuard lock(uoCb_mutex);
  precond(msg_cb);

  for (int i = 0; i < cbs_size; ++i) {
    if (uoCb_cbs[i].cb != msg_cb)
      continue;

    uoCb_cbs_enable[i] = false;
    return true;

  }

  return false;
}

///////////// Publishing /////////////////////////////

static void uoCb_publish(uoCb_cbT cb, const void *ptr, uo_flagsT flags) {
  uoCb_msgT
  msg { .cptr = ptr, .flags = flags };
  cb(msg);
}

// public

void uoCb_publish(uoCb_Idxs idxs, const void *ptr, uo_flagsT flags) {
  /*deadlock*/ //OurLockGuard lock(uoCb_mutex);
  for (auto i = 0; i < idxs.size; ++i) {
    auto cb = uoCb_cbs[idxs.arr[i]].cb;
    if (!cb)
      return; // FIXME: callback was un-subscribed by another task
    uoCb_publish(cb, ptr, flags);
  }
}

void uoCb_publish_wsJson(const char *json) {
  /*deadlock*/ //OurLockGuard lock(uoCb_mutex);
  for (auto const &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.tgt.websocket)
      continue;

    uo_flagsT flags;
    flags.fmt.json = true;
    flags.tgt.websocket = true;

    uoCb_publish(it.cb, json, flags);

  }
}

void uoCb_publish_pinChange(const so_arg_pch_t args) {
  /*deadlock*/ //OurLockGuard lock(uoCb_mutex);
  uo_flagsT flags;
  flags.evt.pin_change = true;

  flags.fmt.raw = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    uoCb_publish(idxs, &args, flags);
  }

  flags.fmt.raw = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    if (sizeof buf > snprintf(buf, sizeof buf, "{\"mcu\":{\"gpio%d\":%d}}", args.gpio_num, args.level))
      uoCb_publish(idxs, buf, flags);
  }
}

void uoCb_publish_ipAddress(const char *ip_addr) {
  /*deadlock*/ //OurLockGuard lock(uoCb_mutex);
  uo_flagsT flags;
  flags.evt.ip_address_change = true;

  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    if (sizeof buf > snprintf(buf, sizeof buf, "{\"mcu\":{\"ipaddr\":\"%s\"}}", ip_addr))
      uoCb_publish(idxs, buf, flags);
  }

#ifdef CONFIG_UOUT_PUBLISH_IP_ADDR_AS_TXT
  flags.fmt.json = false;
  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    if (sizeof buf > snprintf(buf, sizeof buf, "tf: ipaddr: %s;", ip_addr))
      uoCb_publish(idxs, buf, flags);
  }
#endif
}

static void quote_string(char *dst, const char *src) {
  for (const char *p = src; *p; ++p) {
    switch (*p) {
    case '\"':
      *dst++ = '\\';
      *dst++ = '\"';
      break;
    default:
      *dst++ = *p;
      break;
    }
  }

  *dst = '\0';
}

void uoCb_publish_logMessage(const LogMessage &msg) {
  /*deadlock*/ //OurLockGuard lock(uoCb_mutex);
  uo_flagsT flags;
  flags.evt.gen_app_log_message = true;
  if ((int) msg.warn_level > (int) LogMessage::wl_Info)
    flags.evt.gen_app_error_message = true;

  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[256];
    char quoted_txt[strlen(msg.txt) * 2 + 1];
    quote_string(quoted_txt, msg.txt);

    if (int n = snprintf(buf, sizeof buf, "{\"log\":{\"wl\":%d, \"tag\":\"%s\", \"txt\":\"%s\"}}", (int) msg.warn_level, msg.tag, quoted_txt); sizeof buf
        > n) {
      uoCb_publish(idxs, buf, flags);
    }
  }

#ifdef CONFIG_UOUT_PUBLISH_LOG_AS_TXT
  flags.fmt.json = false;
  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[128];
    if (sizeof buf > snprintf(buf, sizeof buf, "tf: log:%d:  %s: %s", (int) msg.warn_level, msg.tag, msg.txt)) {
      uoCb_publish(idxs, buf, flags);
    }
  }
#endif
}
