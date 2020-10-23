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
      bool ip_address_change :1;
      bool async_http_resp :1;

      bool pct_change :1; ///< XXX: application-specific flag
      bool timer_change :1; ///< XXX: application-specific flag
      bool rf_msg_received :1;
      bool valve_change :1; ///< XXX: application-specific flag

      /**
       *  \brief any application state change which would interest user clients
       *   (makes sense with JSON or TXT format)
       *  \note Favor this bit when possible. This way the publishing function can be implemented
       *        directly in the reporting component instead of an "app_uout" subcomponent.
       */
      bool gen_app_state_change :1;

      /**
       * \brief Any application error message which should be forwarded to user clients.
       */
      bool gen_app_error_message :1;

      bool uo_evt_flag_9 :1;
      bool uo_evt_flag_A :1;
      bool uo_evt_flag_B :1;
      bool uo_evt_flag_C :1;
      bool uo_evt_flag_D :1;
      bool uo_evt_flag_E :1;
      bool uo_evt_flag_F :1;  ///< application specific flags in sub-components  are starting from here (highest to lowest)
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

typedef void (*uoCb_cbT)(const uoCb_msgT msg);
constexpr int cbs_size = 8;
struct uoCb_cbsT {
  uoCb_cbT cb;
  uo_flagsT flags;
};

/**
 * \brief  Result type for filtering callbacks
 */
struct uoCb_Idxs {
  uint8_t size;
  uint8_t arr[cbs_size];
};

/**
 * \brief       Filter all call-backs
 * \param flags Exactly one format bit must be set here. A callback will match if any in flags_evt or flags_tgt matches with callback flags.
 * \return      The matching call-backs to be used in calling \linke uoCb_publish \endlink.
 */
uoCb_Idxs uoCb_filter(uo_flagsT flags);

/**
 * \brief       Filter all call-backs
 * \param flags Exactly one format bit must be set here. A callback will match if any in flags_evt or flags_tgt matches with callback flags.
 * \param idxs  The set of call-backs to match from.
 * \return      The matching call-backs to be used in calling \linke uoCb_publish \endlink.
 */
uoCb_Idxs uoCb_filter(uo_flagsT flags, uoCb_Idxs idxs);

/**
 * \brief register a callback to subscribe for published messages
 * \param msg_cb      will be called if matching event occurs
 * \flags flags       describes events you will get
 */
bool uoCb_subscribe(uoCb_cbT msg_cb, uo_flagsT flags);
bool uoCb_unsubscribe(uoCb_cbT msg_cb);


// publishing

/// \brief publish any data
/// \note  Use this to implement your own publish functions
void uoCb_publish(uoCb_Idxs idxs, const void *ptr, uo_flagsT flags);

/// \brief publish json to tgt.websocket subscribers
void uoCb_publish_wsJson(const char *json);

/// \brief publish input pin level to evt.pin_change subscribers
void uoCb_publish_pinChange(const so_arg_pch_t args);

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

