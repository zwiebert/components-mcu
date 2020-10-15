/**
 * \file uout/uo_callbacks.h
 * \brief pub/sub interface to publish asynchronously to MQTT servers, HTTP-client web-sockets, USB-console, TCP-sockets/consoles
 */
#pragma once


#include <stdint.h>
#include <uout/uo_types.h>
#include <type_traits>

/**
 * \brief  flags to address subscribers  by event, data format and target
 */
typedef struct uo_flagsT {


  union {
    /// \brief event types a subscriber wants to be notified about. One callback may handle more than one event.
    struct {
      bool pin_change :1;
      bool pct_change :1;
      bool async_http_resp :1;
      bool timer_change :1;
      bool ip_address_change :1;
      bool rf_msg_received :1;
      bool valve_change :1;
      bool flag_7 :1;
      bool flag_8 :1;
      bool flag_9 :1;
      bool flag_A :1;
      bool flag_B :1;
      bool flag_C :1;
      bool flag_D :1;
      bool flag_E :1;
      bool flag_F :1;
    } evt;
    uint16_t evt_flags = 0;
  };

  /// \brief format which the subscriber wants to receive. The callback will be called once for each available format which was subscribed to.
  union {
    struct {
      bool json :1; ///< msg format is const char * containing json
      bool raw :1; ///< msg format is an event dependent object
      bool txt :1; ///< msg format is const char * containing txt for console output
    } fmt;
    uint8_t fmt_flags = 0;
  };


  union {
    /// \brief target to which the subscriber will forward the event data
    struct {
      bool websocket :1; ///< subscriber is a websocket
      bool usart :1; ///< subscriber is an USART console
      bool tcp_term :1; ///< subscriber is a TCP interactive console or general socket (TODO: maybe we should separate these two?)
      bool mqtt :1;  ///< subscriber is an MQTT client
    } tgt;
    uint8_t tgt_flags = 0;
  };

} uo_flagsT;

static_assert(sizeof (uo_flagsT) == 4);
static_assert(sizeof (uo_flagsT::evt) == 2);


/// \brief  message object described by a void pointer and flags
struct uoCb_msgT {
  union {
    const void *cptr;
    //void *ptr;
  };
  uo_flagsT flags;
};

// subscribing

typedef void (*uoCb_cbT)(const uoCb_msgT msg);

/**
 * \brief register a callback to subscribe for published messages
 * \param msg_cb      will be called if matching event occurs
 * \flags flags       describes events you will get
 */
bool uoCb_subscribe(uoCb_cbT msg_cb, uo_flagsT flags);
bool uoCb_unsubscribe(uoCb_cbT msg_cb);


// publishing

/// \brief publish json to tgt.websocket subscribers
void uoApp_publish_wsJson(const char *json);

/// \brief publish input pin level to evt.pin_change subscribers
void uoApp_publish_pinChange(const so_arg_pch_t args);

/// \brief publish ip address to evt.ip_addr_change subscribers
void uoCb_publish_ipAddress(const char *ip_addr);



// accessing message
inline const char *uoCb_jsonFromMsg(const uoCb_msgT msg) {
  if (msg.flags.fmt.json)
    return static_cast<const char *>(msg.cptr);
  return nullptr;
}

inline const char *uoCb_txtFromMsg(const uoCb_msgT msg) {
  if (msg.flags.fmt.txt)
    return static_cast<const char *>(msg.cptr);
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
