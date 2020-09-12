#pragma once
#include <stdint.h>
#include <uout/status_output.h>
#include <type_traits>

typedef struct
//__attribute__((packed))
uo_flagsT {
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
      bool pin_change :1;
      bool pct_change :1;
      bool async_http_resp :1;
      bool timer_change :1;
    } evt;
    uint8_t evt_flags = 0;
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

static_assert(sizeof (uo_flagsT::evt) == 1);
static_assert(sizeof (uo_flagsT) == 3);

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





// accessing message
inline const char *uoCb_jsonFromMsg(const uoCb_msgT msg) {
  if (msg.flags.fmt.json)
    return static_cast<const char *>(msg.cv_ptr);
  return nullptr;
}


#ifdef UOUT_PROTECTED

constexpr int cbs_size = 6;

struct uoCb_cbsT {
  uoCb_cbT cb;
  uo_flagsT flags;
};

extern uoCb_cbsT uoCb_cbs[cbs_size];


#endif
