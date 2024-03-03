/*
 * http_client.c
 *
 *  Created on: 10.06.2019
 *      Author: bertw
 */

/* ESP HTTP Client Example

 This example code is in the Public Domain (or CC0 licensed, at your option.)

 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */

#include <string.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
extern "C" {
#include "esp_log.h"
}

#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "net/http_client.h"

#include "debug/dbg.h"

static const char *TAG = "HTTP_CLIENT";

static bool httpClient_get(esp_http_client_config_t *config) {
  bool result = true;

  esp_http_client_handle_t client = esp_http_client_init(config);
  esp_err_t err = esp_http_client_perform(client);

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "HTTP chunk encoding Status = %d, content_length = %lld", esp_http_client_get_status_code(client),
        esp_http_client_get_content_length(client));
  } else {
    ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    result = false;
  }
  esp_http_client_cleanup(client);
  return result;

}

bool httpClient_downloadFile(const char *srcUrl, const char *dstFile) {

  esp_http_client_config_t config = { .url = srcUrl, .event_handler = [](esp_http_client_event_t *evt) -> esp_err_t {
    static int fd = -1;

    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
      ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
      break;
    case HTTP_EVENT_ON_DATA:
      if (fd == -1) {
        const char *fileName = (const char*) evt->user_data;
        if (fd >= 0)
          db_printf("write to file %s\n", fileName);

        fd = open(fileName, O_CREAT | O_TRUNC | O_WRONLY);
        if (fd < 0) {
          perror(fileName);
        }
      }

      if (fd >= 0) {
        write(fd, evt->data, evt->data_len);
        db_printf(".");
      } else {
        db_printf("-");
      }
      //ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      if (!esp_http_client_is_chunked_response(evt->client)) {
        // Write out data
        //db_printf("%.*s", evt->data_len, (char*)evt->data);
      }

      break;
    case HTTP_EVENT_ON_FINISH:
      if (fd >= 0) {
        close(fd);
        fd = -1;
      }

      ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
      break;
    case HTTP_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
      break;

    case HTTP_EVENT_REDIRECT:  //TODO
      ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
      break;
    }
    return ESP_OK;
  }, .user_data = (void*) dstFile, };

  return httpClient_get(&config);
}

bool httpClient_getToBuffer(const char *srcUrl, char *buf, size_t buf_size) {
  struct user_data {
    char *buf;
    size_t buf_size;
  } ud = { buf, buf_size };

  esp_http_client_config_t config = { .url = srcUrl, .event_handler = [](esp_http_client_event_t *evt) -> esp_err_t {
    static int buf_pos;

    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
      ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
      break;
    case HTTP_EVENT_ON_DATA: {
      user_data *ud = (user_data*) evt->user_data;
      if (buf_pos + evt->data_len >= ud->buf_size)
        return ESP_ERR_NO_MEM;

      memcpy(ud->buf + buf_pos, evt->data, evt->data_len);
      buf_pos += evt->data_len;
    }
      break;
    case HTTP_EVENT_ON_FINISH: {
      user_data *ud = (user_data*) evt->user_data;
      ud->buf[buf_pos] = '\0';

      ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
    }
      break;
    case HTTP_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
      break;

    case HTTP_EVENT_REDIRECT:  //TODO
      ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
      break;
    }
    return ESP_OK;
  }, .user_data = (void*) &ud, };

  return httpClient_get(&config);
}

