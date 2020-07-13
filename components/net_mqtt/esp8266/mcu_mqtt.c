/* main.c -- MQTT client example
*
* Copyright (c) 2014-2015, Tuan PM <tuanpm at live dot com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Redis nor the names of its contributors may be used
* to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#include "mcu_mqtt.h"
#include "user_config.local.h"
#include "user_config.h"
#include "esp_mqtt/mqtt/include/mqtt.h"
#include "userio/status_output.h"

#include "ets_sys.h"
#include "osapi.h"

#include <stdbool.h>

#define D(x) x
#ifdef USE_MQTT
MQTT_Client mqttClient;
MQTT_Client* client;

static bool is_connected;
bool destroy_on_disconnect;

void io_mqtt_subscribe(const char *topic, int qos) {
  if (!client || !is_connected)
    return;
  /*int msg_id = */ MQTT_Subscribe(client, (char*)topic, qos);
}

void io_mqtt_unsubscribe(const char *topic) {
  if (!client || !is_connected)
    return;

  /*int msg_id = */ MQTT_UnSubscribe(client, (char*)topic);
}

void io_mqtt_publish(const char *topic, const char *data) {
  if (!client || !is_connected)
    return;
  if (!so_tgt_test(SO_TGT_MQTT))
    return;

 // D(ESP_LOGI(TAG, "MQTT_PUBLISH, topic=%s, data=%s", topic, data));
  /*int msg_id = */ MQTT_Publish(client, topic, data, strlen(data), 1, 0);
}


static void mqttConnectedCb(uint32_t *args)
{
  D(io_puts("mqtt: connected\n"));
  is_connected = true;
  io_mqtt_connected();
}

static void mqttDisconnectedCb(uint32_t *args)
{
  D(io_puts("mqtt: disconnected\n"));
  is_connected = false;
  io_mqtt_disconnected();
  if (destroy_on_disconnect) {
    MQTT_DeleteClient(client);
    client = 0;
    is_connected = false;
    destroy_on_disconnect = false;
  }
}

static void mqttPublishedCb(uint32_t *args)
{
  D(io_puts("mqtt: published\n"));
  MQTT_Client* client = (MQTT_Client*)args;
}

static void mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
  D(io_puts("mqtt: received\n"));
  MQTT_Client* client = (MQTT_Client*)args;
  io_mqtt_received(topic, topic_len, data, data_len);
}

static void io_mqtt_stop_and_destroy(void) {
  D(io_puts("mqtt: stop\n"));
  if (client) {
    MQTT_Disconnect(client);
    destroy_on_disconnect = true;
  }
}


static void io_mqtt_create_and_start(struct cfg_mqtt *c, char *host, unsigned port) {
  io_puts("mqtt: starting...\n");
  if (client)
    io_mqtt_stop_and_destroy();

  MQTT_InitConnection(&mqttClient, host, port, DEFAULT_SECURITY);
  if (!MQTT_InitClient(&mqttClient, c->client_id, c->user, c->password, MQTT_KEEPALIVE, MQTT_CLEAN_SESSION)) {
    io_puts("Failed to initialize properly. Check MQTT version.\n");
    return;
  }
  client = &mqttClient;
#if 1
  MQTT_InitLWT(&mqttClient, "/lwt", "offline", 0, 0);
  MQTT_OnConnected(&mqttClient, mqttConnectedCb);
  MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
  MQTT_OnPublished(&mqttClient, mqttPublishedCb);
  MQTT_OnData(&mqttClient, mqttDataCb);
#endif
  MQTT_Connect(client);
  io_puts("mqtt: ...started\n");
}



void io_mqtt_setup(struct cfg_mqtt *c)
{
  if (c && c->enable) {
    unsigned io_mqtt_port = 1883;
    char *io_mqtt_host = 0;

  char *s1=0, *s2=0;
  char *host_start = 0;
  int host_len = 0;
  char *port_start = 0;

  if ((s1 = strstr(c->url, "://"))) {
    host_start = s1 + 3;
    if ((s2 = strchr(host_start, ':'))) {
      host_len = s2 - host_start;
      port_start = s2 + 1;
      } else {
        host_len = strlen(host_start);
      }
  }

  if (port_start)
    io_mqtt_port = atoi(port_start);
  if (host_start) {
    io_mqtt_host = alloca(host_len+1);
    strncpy(io_mqtt_host, host_start, host_len);
    io_mqtt_host[host_len] = '\0';
  }

    io_mqtt_create_and_start(c, io_mqtt_host, io_mqtt_port);
  } else {
    io_mqtt_stop_and_destroy();
  }
}
#endif
