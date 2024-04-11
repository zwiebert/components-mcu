#include <sys/param.h>
#include "net_http_server/http_server_setup.h"
#include "net_http_server/content.hh"
#include "uout/uout_builder_json.hh"
#include "utils_misc/int_types.h"
#include "cli/mutex.hh"
#include "net_http_server/esp32/http_server_esp32.h"

#include <mbedtls/base64.h>
#include <esp_check.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>

#include <fcntl.h>

static const char *TAG = "http_server";
#ifdef CONFIG_NET_HTTP_SERVER_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif

/*
 *
 * Structure holding server handle
 * and internal socket fd in order
 * to use out of request send
 */
struct async_resp_arg {
  httpd_handle_t hd;
  int fd;
};

fd_set ws_fds;
int ws_nfds;
struct ws_send_arg {
  httpd_handle_t hd;
  char *json;
  size_t json_len;
  int fd;
};

void ws_async_broadcast(void *arg) {
  auto a = static_cast<ws_send_arg*>(arg);
  D(ESP_LOGE(TAG, "ws_async_broadcast: json=<%s>, fd=%d", a->json, a->fd));
  httpd_ws_frame_t ws_pkt = { .final = true, .fragmented = false, .type = HTTPD_WS_TYPE_TEXT, .payload = (uint8_t*) a->json, .len = a->json_len };

  for (int fd = 0; fd < ws_nfds; ++fd) {
    if (!FD_ISSET(fd, &ws_fds))
      continue;
    esp_err_t res = httpd_ws_send_frame_async(a->hd, fd, &ws_pkt);
    if (res != ESP_OK) {
      ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d (%s)", res, esp_err_to_name(res));
      FD_CLR(fd, &ws_fds);
      if (ws_nfds == fd + 1)
        ws_nfds = fd;
    }
  }
  free(a->json);
  free(a);
}

esp_err_t ws_trigger_send(httpd_handle_t handle, const char *json, size_t len, int fd) {
  struct ws_send_arg *arg = static_cast<struct ws_send_arg*>(malloc(sizeof(struct ws_send_arg)));
  arg->hd = handle;
  arg->json = strdup(json);
  arg->json_len = len;
  arg->fd = fd;
  return httpd_queue_work(handle, ws_async_broadcast, arg);
}

void ws_send_json(const char *json, ssize_t len) {
  ws_trigger_send(hts_server, json, len >= 0 ? len : strlen(json));
}

int ws_write(void *req, const char *s, ssize_t s_len, int chunk_status) {
  const size_t len = s_len < 0 ? strlen(s) : (size_t) s_len;

  const bool first_chunk = chunk_status == 0 || chunk_status == -1;
  const bool single_chunk = chunk_status == 0;
  const bool final_chunk = 0 <= chunk_status; // 0 or total number of chunks


  D(ESP_LOGE(TAG, "ws_write(%p, <%.*s>, len=%d, chunk=%d", req, (int)s_len, s, (int)s_len, chunk_status));
  httpd_ws_frame_t ws_pkt = {
      .final = final_chunk,
      .fragmented = !single_chunk,
      .type =  first_chunk ? HTTPD_WS_TYPE_TEXT : HTTPD_WS_TYPE_CONTINUE,
      .payload = (uint8_t*) s, .len = len };
  if (auto res = httpd_ws_send_frame((httpd_req_t*) req, &ws_pkt); res != ESP_OK) {
    ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d (%s)", res, esp_err_to_name(res));
    return -1;
  }

  return len;
}

