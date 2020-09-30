#pragma once
#include <stdint.h>
#include <uout/status_output.h>
#include <type_traits>

typedef struct uo_flagsT {
  union {
    struct {
      bool pin_change :1;
      bool pct_change :1;
      bool async_http_resp :1;
      bool timer_change :1;
      bool ip_address_change :1;
      bool rf_msg_received :1;
      bool stm32_update :1;
    } evt;
    uint16_t evt_flags = 0;
  };

  union {
    struct {
      bool json :1;
      bool obj :1;
      bool txt :1;
    } fmt;
    uint8_t fmt_flags = 0;
  };

  union {
    struct {
      bool websocket :1;
      bool usart :1;
      bool tcp_term :1;
      bool mqtt :1;
    } tgt;
    uint8_t tgt_flags = 0;
  };

} uo_flagsT;

static_assert(sizeof (uo_flagsT) == 4);
static_assert(sizeof (uo_flagsT::evt) == 1);


struct uoCb_msgT {
  union {
    const void *cv_ptr;
    //void *v_ptr;
  };
  uo_flagsT flags;
#ifdef __cplusplus
  bool is_type_string() const {
    return (flags.fmt.txt || flags.fmt.json);
  }

  const char* get_string() const {
    if (is_type_string())
      return static_cast<const char*>(cv_ptr);
    return nullptr;
  }
#endif
};

// subscribing

typedef void (*uoCb_cbT)(const uoCb_msgT msg);

bool uoCb_subscribe(uoCb_cbT msg_cb, uo_flagsT flags);
bool uoCb_unsubscribe(uoCb_cbT msg_cb);


// publishing
void uoApp_publish_wsJson(const char *json);
void uoApp_publish_pinChange(const so_arg_pch_t args);
void uoCb_publish_ipAddress(const char *ip_addr);



// accessing message
inline const char *uoCb_jsonFromMsg(const uoCb_msgT msg) {
  if (msg.flags.fmt.json)
    return static_cast<const char *>(msg.cv_ptr);
  return nullptr;
}

inline const char *uoCb_txtFromMsg(const uoCb_msgT msg) {
  if (msg.flags.fmt.txt)
    return static_cast<const char *>(msg.cv_ptr);
  return nullptr;
}


#if 1//def UOUT_PROTECTED

constexpr int cbs_size = 6;

struct uoCb_cbsT {
  uoCb_cbT cb;
  uo_flagsT flags;
};

extern uoCb_cbsT uoCb_cbs[cbs_size];


#endif
