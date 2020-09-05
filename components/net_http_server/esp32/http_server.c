#include "app_config/proj_app_cfg.h"
#include "net/http/server/http_server.h"
#include "userio/status_json.h"
#include "net/http/server/esp32/register_uris.h"
#include "debug/dbg.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include <mbedtls/base64.h>
#include <esp_http_server.h>

static const char *TAG="APP";



extern hts_register_uri_handlers_cbT hts_register_uri_handlers_cb;

//////////////////////////Authorization//////////////////////
static char *auth_user;
static char *auth_password;
#define HTTP_USER (auth_user ? auth_user : "")
#define HTTP_USER_LEN (auth_user ? strlen(auth_user) : 0)
#define HTTP_PW (auth_password ? auth_password : "")
#define HTTP_PW_LEN  (auth_password ? strlen(auth_password) : 0)

static bool verify_userName_and_passWord(const char *up, size_t up_len) {

  if (HTTP_USER_LEN + 1 + HTTP_PW_LEN != up_len)
    return false;

  return strncmp(HTTP_USER, up, HTTP_USER_LEN) == 0 && strncmp(HTTP_PW, up + (up_len - HTTP_PW_LEN), HTTP_PW_LEN) == 0;
}

static bool verify_authorization(httpd_req_t *req) {
  bool login_ok = false;
  char *login = 0;

  size_t login_len = httpd_req_get_hdr_value_len(req, "Authorization");
  if (login_len && (login = malloc(login_len + 1))) {
    esp_err_t err = httpd_req_get_hdr_value_str(req, "Authorization", login, login_len + 1);
    if (err == ESP_OK) {
      unsigned char dst[128];
      size_t olen = 0;
      if (0 == mbedtls_base64_decode(dst, sizeof dst, &olen, (unsigned char*) login + 6, login_len - 6))
        login_ok = verify_userName_and_passWord((char*)dst, olen);
    }

    free(login);
  }

  return login_ok;
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
  if (strcmp(HTTP_USER, c->user) != 0) {
    free(auth_user);
    if ((auth_user = malloc(strlen(c->user)+1)))
      strcpy(auth_user, c->user);
  }
  if (strcmp(HTTP_PW, c->password) != 0) {
    free(auth_password);
    if ((auth_password = malloc(strlen(c->password)+1)))
      strcpy(auth_password, c->password);
  }

  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_open_sockets = 6;
  config.uri_match_fn = httpd_uri_match_wildcard;

  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) != ESP_OK) {
    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
  }

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


