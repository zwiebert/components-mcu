/*
 * wifi.h
 *
 *  Created on: 20.03.2019
 *      Author: bertw
 */

#pragma once

#include "stdbool.h"
// interface implemented by mcu specific code
void ipnet_addr_as_string(char *buf, unsigned buf_len);

// interface called by mcu specific code
void ipnet_connected(void);
void ipnet_disconnected(void);
bool ipnet_isConnected(void);

uint32_t get_ip4addr_local();
uint32_t get_ip4addr_gateway();
uint32_t get_ip4netmask();

typedef void (*ipnet_cb)(void);
void ipnet_cbRegister_gotIpAddr(ipnet_cb cb);
void ipnet_cbRegister_lostIpAddr(ipnet_cb cb);



extern ipnet_cb ipnet_gotIpAddr_cb, ipnet_lostIpAddr_cb;

