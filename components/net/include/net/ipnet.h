/**
 * \file   net/ipnet.h
 * \brief  Getting some info about our IP connection
 */
#ifdef __cplusplus
  extern "C" {
#endif

#pragma once

#include "stdbool.h"
#include <stdint.h>

// interface implemented by mcu specific code
void ipnet_addr_as_string(char *buf, unsigned buf_len);

// interface called by mcu specific code

extern void (*ipnet_CONNECTED_cb)();

/**
 * \brief Event: Got connected to IP
 * \note  Needs to be called from main-thread!
 */
void ipnet_connected(void);

/**
 * \brief Event: Got disconnected from IP
 * \note  Needs to be called from main-thread!
 */
void ipnet_disconnected(void);


bool ipnet_isConnected(void); ///< Test if connection to IP exists
uint32_t get_ip4addr_local(); ///< Get local IP4 address
uint32_t get_ip4addr_gateway(); ///< Get gateway IP4 address
uint32_t get_ip4netmask(); ///< Get IP4 net-mask


typedef void (*ipnet_cb)(void); ///< Callback function pointer type

/**
 * \brief    Register callback to get notified about getting a new IP address (XXX)
 * \param cb Callback function pointer
 */
void ipnet_cbRegister_gotIpAddr(ipnet_cb cb);
/**
 * \brief Register callback to get notified about losing IP address (XXX)
 * \param cb Callback function pointer
 */
void ipnet_cbRegister_lostIpAddr(ipnet_cb cb);



extern ipnet_cb ipnet_gotIpAddr_cb, ipnet_lostIpAddr_cb;

#ifdef __cplusplus
  }
#endif
