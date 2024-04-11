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
 * \brief serve response data according to file_map matching the URI
 */
esp_err_t respond_file(httpd_req_t *req, const struct file_map *fm) {

  auto set_hdrs = [req, fm]() -> esp_err_t {
    if (fm->type && ESP_OK != httpd_resp_set_type(req, fm->type))
      return ESP_FAIL;
    if (fm->wc.content_encoding && ESP_OK != httpd_resp_set_hdr(req, "content-encoding", fm->wc.content_encoding))
      return ESP_FAIL;

    return ESP_OK;
  };

  // serve data provided by a content_reader (like vfs-files, etc)
  if (fm->content_reader) {
    struct open_file_to_read {
      open_file_to_read(httpd_req_t *req, const struct file_map *fm) :
          m_fm(fm) {
        char buf[128];
        bool isQuery = ESP_OK == httpd_req_get_url_query_str(req, buf, sizeof buf);
        fd = fm->content_reader->open(fm->wc.content, isQuery ? buf : nullptr);
      }
      ~open_file_to_read() {
        m_fm->content_reader->close(fd);
      }
      const struct file_map *m_fm;
      int fd;
    };

    open_file_to_read of(req, fm);
    if (of.fd < 0) {
      return ESP_FAIL;
    }

    for (int i = 0;; ++i) {
      char buf[256];
      auto bytes_read = fm->content_reader->read(of.fd, &buf[0], sizeof buf);

      // handle read error
      if (bytes_read < 0) {
        ESP_LOGE("respond_file", "read error");
        if (!(ESP_OK == set_hdrs() && ESP_OK == httpd_resp_send_chunk(req, "", 0))) { //FIXME return error
          return ESP_FAIL;
        }
        return ESP_FAIL;
      }

      ESP_LOGI("content_reader", "bytes_read=<%u>", bytes_read);

#if 0 // this optimization would make it harder upstream if data is originally created in chunks
      // If EOF, then send whole buffer and return
      if (i == 0 && bytes_read < sizeof buf) {
        int br = fm->content_reader->read(of.fd, &buf[bytes_read], 1);
        if (br == 0)
          return (ESP_OK == set_hdrs() && ESP_OK == httpd_resp_send(req, buf, bytes_read)) ? ESP_OK : ESP_FAIL;
        if (br > 0)
          bytes_read += br;
      }
#endif

      // send chunks. last chunk needs to have size zero
      if (!(ESP_OK == set_hdrs() && ESP_OK == httpd_resp_send_chunk(req, buf, bytes_read))) {
        return ESP_FAIL;
      }

      if (bytes_read == 0)
        return ESP_OK;
    }

    return ESP_FAIL; // this line should be unreachable
  }

// serve memory block
  if (fm->wc.content && fm->wc.content_length) {
    return (ESP_OK == set_hdrs() && ESP_OK == httpd_resp_send(req, fm->wc.content, fm->wc.content_length)) ? ESP_OK : ESP_FAIL;
  }

// serve null terminated string
  if (fm->wc.content && !fm->wc.content_length) {
    return (ESP_OK == set_hdrs() && ESP_OK == httpd_resp_sendstr(req, fm->wc.content)) ? ESP_OK : ESP_FAIL;
  }

  return ESP_FAIL; // nothing did match
}

