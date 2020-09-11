#pragma once
#include <stdint.h>
#include <uout/status_output.h>
#include <type_traits>

typedef uint8_t uoCb_msgTypeT;
typedef uint16_t uoCb_flagT;

enum class uoCb_msgTypes : uoCb_msgTypeT { NONE, str, json, END };
enum class uoCB_msgFlagBits : uoCb_flagT { ws, END };

#define UOCB_MT(v) static_cast<std::underlying_type<uoCb_msgTypes>::type>(uoCb_msgTypes:: v)
#define UOCB_MFB(v) static_cast<std::underlying_type<uoCB_msgFlagBits>::type>(uoCB_msgFlagBits:: v)


inline bool operator==(uoCb_msgTypes lhs, uoCb_msgTypeT rhs) {
  return static_cast<uoCb_msgTypeT>(lhs) == rhs;
}

struct uoCb_msgT {
  union {
    const void *cv_ptr;
    //void *v_ptr;
  };
  uoCb_msgTypeT msg_type;
};

inline bool uoCb_isStr(const uoCb_msgT *msg) { return UOCB_MT(str) == msg->msg_type; }
inline bool uoCb_isJson(const uoCb_msgT *msg) { return UOCB_MT(json) == msg->msg_type; }

typedef void (*uoCb_cbT)(const uoCb_msgT *msg);

bool uoCb_register_callback(uoCb_cbT msg_cb, uoCb_flagT flags);
bool uoCb_unregister_callback(uoCb_cbT msg_cb);

void uoApp_event_wsJson(const char *json);

inline const char *uoCb_jsonFromMsg(const uoCb_msgT *msg) {
  if (uoCb_msgTypes::json == msg->msg_type)
    return static_cast<const char *>(msg->cv_ptr);
  return nullptr;
}


#ifdef UOUT_PROTECTED

constexpr int cbs_size = 6;

struct uoCb_cbsT {
  uoCb_cbT cb;
  uoCb_flagT flags;
};

extern uoCb_cbsT uoCb_cbs[cbs_size];

#endif
