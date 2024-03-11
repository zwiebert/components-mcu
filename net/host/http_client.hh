#include <cstddef>
#include <iostream>
#include <regex>
#include <string>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

//#define DEBUG

#ifdef DEBUG
using namespace std;
#define D(x) x
#else
#define D(x)
#endif

template<std::size_t buf_size>
struct rbuf {
private:
  std::string m_scheme;
  std::string m_host_name;
  std::string m_url_data;
  std::string m_port;
  int m_sock = -1;
  char m_buf[buf_size];
  unsigned m_buf_idx = 0;

  char *m_header = nullptr;
  char *m_body = nullptr;
  std::size_t m_body_size = 0;
  char *m_resp_buf = &m_buf[0];
  std::size_t m_resp_buf_size = buf_size;
  int m_resp_buf_idx = 0;

  bool put(char c) {
    if ((m_buf_idx + 1) >= buf_size)
      return false;

    m_buf[m_buf_idx++] = c;
    m_buf[m_buf_idx] = '\0'; // ???
    return true;
  }
  bool put(const char *s) {
    while (*s) {
      if (!put(*s++))
        return false;
    }
    return true;
  }
  bool put(const std::string &s) {
    for (auto c : s) {
      put(c);
    }
    return true;
  }
public:
  /// host-name from URL
  const char* get_host_name() const {
    return m_host_name.c_str();
  }
  /// port from URL or 80 as default
  const char* get_port() const {
    return m_port.c_str();
  }
  /// header lines from HTTP response
  const char* get_header_lines() const {
    return m_header;
  }
  /// content length from HTTP response
  unsigned get_body_length() const {
    return m_body_size;
  }
  /// content from HTTP response (null terminated hopefully)
  const char* get_body() const {
    return m_body;
  }

  /**
   * \brief  use URL to fetch content into a buffer
   * \param  buffer      pointer to buffer provided by user. If null use internal buffer.
   * \param  buffer_size  size of buffer
   * \return pointer to content body.  null on failure
   */
  char* fetch(const char *url, const char *accept_content = "application/json", char *buffer = nullptr, size_t buffer_size = 0) {
    auto &rb = *this;
    char *result = nullptr;

    // use buffer provided by user for this call only
    if (buffer) {
      m_resp_buf = buffer;
      m_resp_buf_size = buffer_size;
    }

    if (rb.parse_url(url)) {
      if (rb.open_connection()) {
        D(std::cout << "connection opened\n");

        if (rb.build_request_GET(accept_content)) {
          D(std::cout << "request build\n");
          if (rb.send_request()) {
            D(std::cout << "request sent\n");
            if (rb.get_response()) {
              D(std::cout << "got response\n");
              if (rb.separate_header_and_body()) {
                D(std::cout << "separated headers and content\n");
                rb.close_connection();
#ifdef DEBUG
            cout << "Headers:\n" << rb.m_header << "\n";
            cout << "Body:\n" << rb.m_body << "\n";
            cout << "BodySize: " << rb.m_body_size << "\n";
#endif
                result = m_body;
              }
            }
          }
        }
      }
    }
    // switch back to internal buffer
    if (buffer) {
      m_resp_buf = m_buf;
      m_resp_buf_size = buf_size;
    }
    return result;
  }

public:
  /**
   * \brief   Parse URL and save for later work
   */
  bool parse_url(const char *url) {
    std::regex url_regex("^(?:([^:/?#]+):)?(?://([^:/?#]+)(?::(\\d+))?)?([^#]*)");
    std::cmatch url_match;
    std::regex_match(url, url_match, url_regex);
    int err = 0;

    if (auto m = url_match[1]; m.matched) {
      m_scheme = std::csub_match(m).str();
    } else {
      m_scheme = "http";
    }

    if (auto m = url_match[2]; m.matched) {
      m_host_name = std::csub_match(m).str();
    }

    if (auto m = url_match[3]; m.matched) {
      m_port = std::csub_match(m).str();
    } else {
      m_port = "80";
    }

    if (auto m = url_match[4]; m.matched) {
      m_url_data = std::csub_match(m).str();
    }

#ifdef DEBUG
    for (std::size_t i = 0; i < url_match.size(); ++i) {
      std::csub_match sub_match = url_match[i];
      std::string piece = sub_match.str();
      std::cout << "  submatch " << i << ": " << piece << '\n';
    }
#endif

    return err == 0;
  }

  /**
   * \brief   Buildd a GET request based on previously parsed URL
   */
  bool build_request_GET(const char *hdr_accept = "application/json") {
    m_buf_idx = 0;
    put("GET ");
    put(m_url_data);
    put("  HTTP/1.1\r\n"
        "Host: ");
    put(m_host_name);
    put("\r\n"
        "User-Agent: Mozilla/4.0\r\n"
        "Accept: ");
    put(hdr_accept);
    put("\r\n"
        "Connection: close\r\n"
        "\r\n"
        "\r\n");

    return true;
  }

  /// send previously built request
  bool send_request() {
    int written = write(m_sock, m_buf, m_buf_idx);
    return written == m_buf_idx;
  }

  /// read response after send_request()
  bool get_response() {
    m_resp_buf_idx = 0;
    unsigned buf_size_remaining = m_resp_buf_size;
    for (char *p = m_resp_buf; m_resp_buf_idx < m_resp_buf_size;) {
      if (int ct = read(m_sock, p, buf_size_remaining - 1); ct > 0) {
        m_resp_buf_idx += ct;
        buf_size_remaining -= ct;
        p += ct;
        *p = '\0';
      } else {
        return ct == 0 && m_resp_buf_idx;
      }
    }
    return true;
  }

  ///  find where the content starts in response and null terminate headers
  bool separate_header_and_body() {
    const char *ok_string ="HTTP/1.1 200 OK\r\n";
    if (0 != strncmp(m_resp_buf, ok_string, strlen(ok_string))) {
      m_header = m_resp_buf;
      m_body = nullptr;
      return false;
    }

    if (auto sep = strstr(m_resp_buf, "\r\n\r\n"); sep) {
      m_header = m_resp_buf;
      sep[2] = '\0';
      m_body = sep + 4;
      m_body_size = m_resp_buf_idx - (sep - m_resp_buf) - 4;
      return true;
    }
    return false;
  }

  /// open a connection to the host from previously parsed URL
  bool open_connection() {
    if (m_host_name == "" || m_port == "")
      return false;
    m_sock = do_connect(m_host_name.c_str(), m_port.c_str());
    return m_sock >= 0;
  }

  /// close opened connection
  void close_connection() {
    shutdown(m_sock, SHUT_RDWR);
    close(m_sock);
    m_sock = -1;
  }
private:
  static constexpr size_t BUF_SIZE = 500;
  static int do_connect(const char *host_name, const char *service) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    size_t len;
    ssize_t nread;
    char buf[BUF_SIZE];

    /* Obtain address(es) matching host/port */

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_TCP;

    s = getaddrinfo(host_name, service, &hints, &result);
    if (s != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
      return -1;
    }

    /* getaddrinfo() returns a list of address structures.
     Try each address until we successfully connect(2).
     If socket(2) (or connect(2)) fails, we (close the socket
     and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
      sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sfd == -1)
        continue;

      if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
        break; /* Success */

      close(sfd);
    }

    freeaddrinfo(result); /* No longer needed */

    if (rp == NULL) { /* No address succeeded */
      fprintf(stderr, "Could not connect\n");
      return -1;
    }

    return sfd;
  }

};
