#include <utils_misc/int_macros.h>
#include <uout/uo_callbacks.h>

#include <stdio.h>

static uoCb_cbsT uoCb_cbs[cbs_size]; ///< store call-back pointers
static uoCb_Idxs uoCb_cbs_idxs; ///< keep indexes of all currently registered call-backs

/////////////////// Filter /////////////////////////////////////

// public

uoCb_Idxs uoCb_filter(uo_flagsT flags, uoCb_Idxs idxs) {
  uoCb_Idxs result { };

  for (auto i = 0; i < uoCb_cbs_idxs.size; ++i) {
    auto &it = uoCb_cbs[uoCb_cbs_idxs.arr[i]];
    if (0 == (it.flags.evt_flags & flags.evt_flags) && 0 == (it.flags.tgt_flags & flags.tgt_flags))
      continue;
    if (0 == (it.flags.fmt_flags & flags.fmt_flags))
      continue;

    result.arr[result.size++] = &it - &uoCb_cbs[0];
  }
  return result;
}

uoCb_Idxs uoCb_filter(uo_flagsT flags) {
  return uoCb_filter(flags, uoCb_cbs_idxs);
}

/////////////////////// Subscriptions /////////////////////////////

static void uoCb_update_idxs() {
  uoCb_cbs_idxs.size = 0;
  for (auto &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    uoCb_cbs_idxs.arr[uoCb_cbs_idxs.size++] = &it - &uoCb_cbs[0];
  }
}

// public

bool uoCb_subscribe(uoCb_cbT msg_cb, uo_flagsT flags) {
  for (auto &it : uoCb_cbs) {
    if (it.cb) {
      continue;
    }
    it.cb = msg_cb;
    it.flags = flags;
    uoCb_update_idxs();
    return true;
  }
  return false;
}

bool uoCb_unsubscribe(uoCb_cbT msg_cb) {
  for (auto &it : uoCb_cbs) {
    if (it.cb != msg_cb)
      continue;
    it.cb = nullptr;
    it.flags.evt_flags = it.flags.fmt_flags = it.flags.tgt_flags = 0;
    uoCb_update_idxs();
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
  for (auto i = 0; i < idxs.size; ++i) {
    uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, ptr, flags);
  }
}

void uoCb_publish_wsJson(const char *json) {
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
    if (sizeof buf >= snprintf(buf, sizeof buf, "{\"mcu\":{\"gpio%d\":%d}}", args.gpio_num, args.level))
      uoCb_publish(idxs, buf, flags);
  }
}

void uoCb_publish_ipAddress(const char *ip_addr) {
  uo_flagsT flags;
  flags.evt.ip_address_change = true;

  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    if (sizeof buf >= snprintf(buf, sizeof buf, "{\"mcu\":{\"ipaddr\":\"%s\"}}", ip_addr))
      uoCb_publish(idxs, buf, flags);
  }

  flags.fmt.json = false;
  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    if (sizeof buf >= snprintf(buf, sizeof buf, "tf: ipaddr: %s;", ip_addr))
      uoCb_publish(idxs, buf, flags);
  }
}

void uoCb_publish_logMessage(const LogMessage msg) {
  uo_flagsT flags;
  flags.evt.gen_app_log_message = true;
  if ((int) msg.warn_level > (int) LogMessage::wl_Info)
    flags.evt.gen_app_error_message = true;

  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[128];
    if (int n = snprintf(buf, sizeof buf, "{\"log\":{\"wl\":%d, \"tag\":\"%s\", \"txt\":\"%s\"}}", (int) msg.warn_level, msg.tag, msg.txt); sizeof buf >= n) {
      // XXX: quote special characters in msg.txt!
      uoCb_publish(idxs, buf, flags);
    }
  }

  flags.fmt.json = false;
  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[128];
    if (sizeof buf >= snprintf(buf, sizeof buf, "tf: log:%d:  %s: %s", (int) msg.warn_level, msg.tag, msg.txt)) {
      uoCb_publish(idxs, buf, flags);
    }
  }
}
