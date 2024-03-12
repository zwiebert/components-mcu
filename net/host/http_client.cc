#include "net/http_client.h"
#include "./http_client.hh"

#include <string.h>

bool httpClient_getToBuffer(const char *url, char *buf, size_t buf_size) {
  static rbuf<1024 * 10> rb; // buffer for headers + content
  if (auto body = rb.fetch(url, "*/*"); body != nullptr) {
    size_t len = rb.get_body_length();
    if (len >= buf_size)
      return false;
    memcpy(buf, body, len);
    buf[len] = '\0';
    return true;
  }
  return false;
}
