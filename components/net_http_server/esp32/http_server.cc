
#include "app_config/proj_app_cfg.h"
#include "../http_server_impl.h"
#include "net_http_server/http_server_setup.h"
#include "uout/status_json.hh"
#include "net_http_server/esp32/http_server_esp32.h"
#include "debug/dbg.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include <mbedtls/base64.h>
#include <esp_http_server.h>
#include <utils_misc/cstring_utils.hh>

static const char *TAG="APP";



//////////////////////////Authorization//////////////////////
#define MAX_LOGIN_LEN 256

static csu auth_user, auth_password;

#define HTTP_USER (auth_user)
#define HTTP_USER_LEN (strlen(auth_user))
#define HTTP_PW (auth_password)
#define HTTP_PW_LEN  (strlen(auth_password))

static bool verify_userName_and_passWord(const char *up, size_t up_len) {

  if (HTTP_USER_LEN + 1 + HTTP_PW_LEN != up_len)
    return false;

  return strncmp(HTTP_USER, up, HTTP_USER_LEN) == 0 && strncmp(HTTP_PW, up + (up_len - HTTP_PW_LEN), HTTP_PW_LEN) == 0;
}

static bool verify_authorization(httpd_req_t *req) {

  size_t login_len = httpd_req_get_hdr_value_len(req, "Authorization");

  if (login_len == 0)
    return false;

  if (login_len > MAX_LOGIN_LEN)
    return false;

  char login[login_len + 1];
  esp_err_t err = httpd_req_get_hdr_value_str(req, "Authorization", login, login_len + 1);
  if (err == ESP_OK) {
    unsigned char dst[128];
    size_t olen = 0;
    if (0 == mbedtls_base64_decode(dst, sizeof dst, &olen, (unsigned char*) login + 6, login_len - 6))
      return verify_userName_and_passWord((char*) dst, olen);
  }

  return false;
}

static bool is_access_allowed(httpd_req_t *req) {
  return (*HTTP_USER == '\0' && *HTTP_PW == '\0') || verify_authorization(req);
}

static void reqest_authorization(httpd_req_t *req) {
  httpd_resp_set_status(req, "401 Unauthorized");
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"tronferno-mcu.all\"");
  httpd_resp_sendstr(req, "<p>please login</p>");
}

bool check_access_allowed(httpd_req_t *req) {
  if (!is_access_allowed(req)) {
    reqest_authorization(req);
   return false;
  }
  return true;
}

static httpd_handle_t start_webserver(struct cfg_http *c) {
  auth_user = c->user;
  auth_password = c->password;

  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_open_sockets = 6;
  config.uri_match_fn = httpd_uri_match_wildcard;

  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) != ESP_OK) {
    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
  }
  if (hts_register_uri_handlers_cb)
    hts_register_uri_handlers_cb(server);
  return server;
}

///////// public interface ///////////////////
httpd_handle_t hts_server;
void hts_enable_http_server(struct cfg_http *c) {


  if (c && c->enable) {
    if (!hts_server)
      hts_server = start_webserver(c);
  } else {
    if (hts_server) {
      httpd_stop(hts_server);
      hts_server = NULL;
    }
  }
}


