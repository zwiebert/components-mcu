#include "net/tcp_cli_server_setup.hh"
#include "net/tcp_cli_server.h"

#include "app_config/proj_app_cfg.h"
#include "cli/cli.h"
#include "cli/mutex.hh"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "utils_misc/int_types.h"
#include "net_http_server/http_server_setup.h"

#include "time.h"
#include "txtio/inout.h"
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <uout/uo_callbacks.h>

#include <utils_misc/mutex.hh>
#include <utils_misc/new_malloc.hh>
#include <app_config/proj_app_cfg.h>

#define TAG "tcps"

#define printf(...) io_printf_v(vrb3, __VA_ARGS__)
#define perror(s)   io_printf_v(vrb3, "%s: %s\n", s, strerror(errno))

#ifndef DISTRIBUTION
#define DP(x) printf("db:tcps: %s\n", (x))
#define D(x)
#else
#define D(x)
#endif

#ifndef TCPS_TASK_PORT
#define TCPS_TASK_PORT   7777
#endif
#ifndef TCPS_TASK_PORT_IA // Interactive TCP Server (Telnet)
#define TCPS_TASK_PORT_IA   0
#endif

#define TCPS_CCONN_MAX 5

static void callback_subscribe();
static void callback_unsubscribe();
static int tcps_getc();

static TaskHandle_t xHandle = NULL;
#define STACK_SIZE  4096

class TcpCliServer {
public:
  typedef void (TcpCliServer::*fd_funT)(int fd, void *args);
public:
  TcpCliServer(unsigned port = TCPS_TASK_PORT, unsigned port_ia = TCPS_TASK_PORT_IA) :
      m_port(port), m_port_ia(port_ia) {

  }
  ~TcpCliServer() {
    free(buf.buf);
  }
public:
  void set_sockfd(int fd) {
    if (fd + 1 > nfds)
      nfds = fd + 1;
    sockfd = fd;
  }
  void set_sockfd_ia(int fd) {
    if (fd + 1 > nfds)
      nfds = fd + 1;
    sockfd_ia = fd;
  }

  void add_fd(int fd) {
    FD_SET(fd, &wait_fds);
    if (fd + 1 > nfds)
      nfds = fd + 1;
    if (0 == cconn_count++)
      callback_subscribe();
  }

  void rm_fd(int fd) {
    if (!FD_ISSET(fd, &wait_fds)) {
      D(printf("tcp_cli.rm_fd: fd already removed: %d\n", fd));
      return; // XXX
    }
    FD_CLR(fd, &wait_fds);
    if (fd - 1 == nfds)
      --nfds;

    if (--cconn_count <= 0) {
      cconn_count = 0;
      callback_unsubscribe();
    }
  }

  void tcps_close_cconn(int fd) {
    if (lwip_close(fd) < 0) {
      perror("close");
      return;
    }

    rm_fd(fd);
    printf("tcps: disconnected. %d client(s) still connected\n", cconn_count);
  }

  int foreach_fd(fd_set *fdsp, int count, fd_funT fd_fun, void *args) {
    for (int i = 0; count && i < nfds; ++i) {
      if (FD_ISSET(i, fdsp)) {
        (this->*fd_fun)(i, args);
        --count;
      }
    }
    return count;
  }

  int tcps_create_server(int port_number) {
    int fd;
    struct sockaddr_in self = {};
    /** Create streaming socket */
    if ((fd = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Socket");
      return (errno);
    }
    if (lwip_fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
      perror("fcntl");
      goto err;
    }
    /** Initialize address/port structure */
    bzero(&self, sizeof(self));
    self.sin_family = AF_INET;
    self.sin_port = htons(port_number);
    self.sin_addr.s_addr = INADDR_ANY;

    /** Assign a port number to the socket */
    if (lwip_bind(fd, (struct sockaddr*) &self, sizeof(self)) != 0) {
      perror("socket:bind()");
      goto err;
    }

    /** Make it a "listening socket". Limit to 16 connections */
    if (lwip_listen(fd, TCPS_CCONN_MAX) != 0) {
      perror("socket:listen()");
      goto err;
    }

    return fd;

    err: lwip_close(fd);
    return -1;
  }

  int tcps_create_server() {
    const int fd = tcps_create_server(m_port);
    if (fd < 0)
      return (errno);

    set_sockfd(fd);
    return 0;
  }

  int tcps_create_server_ia() {
    const int fd = tcps_create_server(m_port_ia);
    if (fd < 0)
      return (errno);

    set_sockfd_ia(fd);
    return 0;
  }

  int tcpst_putc(int fd, char c) {
    if (lwip_write(fd, &c, 1) < 0) {
      tcps_close_cconn(fd);
      return -1;
    }
    return 1;
  }

  int tcpst_putc_crlf(int fd, char c) {
    if (c == '\r')
      return 1;
    if (c == '\n') {
      if (tcpst_putc(fd, '\r') < 0)
        return -1;
    }
    return tcpst_putc(fd, c);
  }

  void tcpst_putcp_crlf(int fd, void *c) {
    tcpst_putc_crlf(fd, *(char*) c);
  }

  void tcpst_putc_all(char c) {
    foreach_fd(&wait_fds, nfds, &TcpCliServer::tcpst_putcp_crlf, &c);
  }

  void tcpst_writeln(int fd, void *p) {
    const char *s = (const char *)p;
    size_t s_len = strlen(s);

    if (lwip_write(fd, s, s_len) < 0) {
      tcps_close_cconn(fd);
    }
  }

  void tcpst_writeln_all(const char *s) {
    foreach_fd(&wait_fds, nfds, &TcpCliServer::tcpst_writeln, (void*)s);
  }

  int tcps_getc() {
    int result = -1;
    static int fd = -1;
    if (selected_fd >= 0) {
      fd = selected_fd;
    }
    char c;
    int n = lwip_recv(fd, &c, 1, MSG_DONTWAIT);

    if (n == 0) {
      if (selected_fd >= 0) {
        // remote socket was closed
        tcps_close_cconn(fd);
      }
    } else if (n == 1) {
      result = c;
    } else {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {

      } else {
        rm_fd(fd);
      }
    }

    selected_fd = -1;
    return result;
  }

  int try_accept(int socket_fd) {
    int fd;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    /** accept an incoming connection  */
    fd = lwip_accept(socket_fd, (struct sockaddr*) &client_addr, &addrlen);
    if (fd >= 0) {
      add_fd(fd);
      printf("%s:%d connected (%d clients)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), cconn_count);
    } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("tcps: accept");
    }
    return fd;
  }

  void handle_input(int fd, void *args) {
    selected_fd = fd;
    for (;;) {
      switch (cli_get_commandline(&buf, ::tcps_getc)) {
      case CMDL_DONE: {
        LockGuard lock(cli_mutex);
        if (buf.buf[0] == '{') {
          cli_process_json(buf.buf, TargetDescCon { fd, static_cast<so_target_bits>(SO_TGT_CLI | SO_TGT_FLAG_JSON) });
        } else {
          cli_process_cmdline(buf.buf, TargetDescCon { fd, static_cast<so_target_bits>(SO_TGT_CLI | SO_TGT_FLAG_TXT) });
        }
      }
        break;

      case CMDL_INCOMPLETE:
        break;
      case CMDL_LINE_BUF_FULL:
        if (cliBuf_enlarge(&buf))
          continue;
        break;
      case CMDL_ERROR:
        break;
      }
      return;
    }
  }

  void wait_for_fd() {
    fd_set rfds = wait_fds;
    int n = nfds;
    //fd_set wfds = wait_fds;
    //struct timeval tv = { .tv_sec = 10 };

    FD_SET(sockfd, &rfds);
    FD_SET(sockfd_ia, &rfds);
#ifdef CONFIG_APP_USE_CLI_TASK_EXP
     FD_SET(STDIN_FILENO, &rfds);
     if (n <= STDIN_FILENO)
       n = STDIN_FILENO+1;
   #endif

    int count = lwip_select(n, &rfds, NULL, NULL, NULL);
    if (count < 0) {
      return;
    }
    if (count == 0) {
      return;
    }
    D(printf("select returned. count=%d\n", count));

    if (FD_ISSET(sockfd, &rfds)) {
      const int new_fd = try_accept(sockfd);
      FD_CLR(sockfd, &rfds);
      --count;
      if (new_fd >= 0) {
        const char welcome[] = "Type help; for a list of commands\r\n";
        ::write(new_fd, welcome, sizeof welcome);
      }
    }
    if (FD_ISSET(sockfd_ia, &rfds)) {
      const int new_fd = try_accept(sockfd_ia);
      FD_CLR(sockfd_ia, &rfds);
      --count;
      if (new_fd >= 0) {
        const char welcome[] = "Type help; for a list of commands\r\n";
        ::write(new_fd, welcome, sizeof welcome);
      }
    }
#ifdef CONFIG_APP_USE_CLI_TASK_EXP
     if (FD_ISSET(STDIN_FILENO, &rfds)) {
       cli_loop();

       FD_CLR(STDIN_FILENO, &rfds);
       --count;
     }
   #endif
    count = foreach_fd(&rfds, count, &TcpCliServer::handle_input, 0);

  }

  void tcps_task(void *pvParameters) {

    FD_ZERO(&wait_fds);
    nfds = 0;

    if (m_port > 0 && tcps_create_server() == 0) {
      ESP_LOGI(TAG, "tcp server created on port %d", m_port);
    }
    if (m_port_ia > 0 && tcps_create_server_ia() == 0) {
      ESP_LOGI(TAG, "tcp server created on port %d", m_port_ia);
    }
    for (;;) {
      wait_for_fd();
    }
  }

#define USE_WRITELN

  void pctChange_cb(const uoCb_msgT msg) {
    LockGuard lock(tcpCli_mutex);

    if (auto txt = uoCb_txtFromMsg(msg)) {
#ifdef CONFIG_APP_USE_WRITELN
      tcpst_writeln_all(txt);
      tcpst_writeln_all("\r\n");
#else
      for (; *txt; ++txt)
        tcpst_putc_all(*txt);
#endif
    }
    if (auto json = uoCb_jsonFromMsg(msg)) {
#ifdef CONFIG_APP_USE_WRITELN
      tcpst_writeln_all(json);
      tcpst_writeln_all("\r\n");
#else
      for (; *json; ++json)
        tcpst_putc_all(*json);
      tcpst_putc_all('\n');
#endif
    }
  }

private:
  const unsigned m_port;
  const unsigned m_port_ia;
  int sockfd = -1;
  int sockfd_ia = -1;
  int selected_fd = -1;
  fd_set wait_fds = { };
  int nfds = 0;
  int cconn_count = 0;
  struct cli_buf buf = { };
public:
  RecMutex tcpCli_mutex;
};

TcpCliServer *tcp_cli_server;

static int tcps_getc() {
  return tcp_cli_server->tcps_getc();
}

void tcps_task(void *pvParameters) {
  tcp_cli_server->tcps_task(pvParameters);
}

void pctChange_cb(const uoCb_msgT msg) {
  tcp_cli_server->pctChange_cb(msg);
}

static uo_flagsT UserFlags;

/// output callbacks
static void callback_subscribe() {
  uo_flagsT flags = UserFlags;
  flags.evt.pin_change = true;
  flags.evt.gen_app_state_change = true;
  flags.evt.gen_app_error_message = true;
  flags.evt.gen_app_log_message = true;
  flags.fmt.json = true;
  flags.fmt.txt = true;
  uoCb_subscribe(pctChange_cb, flags);
}
static void callback_unsubscribe() {
  uoCb_unsubscribe(pctChange_cb);
}

void tcpCli_setup_task(const struct cfg_tcps *cfg_tcps) {
  static uint8_t ucParameterToPass;

  if (cfg_tcps)
    UserFlags = cfg_tcps->flags;

  if (!cfg_tcps || !cfg_tcps->enable) {
    if (xHandle) {
      vTaskDelete(xHandle);
      xHandle = NULL;
      delete (tcp_cli_server);
      tcp_cli_server = 0;
    }
    return;
  }

  tcp_cli_server = new TcpCliServer();
  xTaskCreate(tcps_task, "tcp_server", STACK_SIZE, &ucParameterToPass, tskIDLE_PRIORITY, &xHandle);
  configASSERT( xHandle );

}

