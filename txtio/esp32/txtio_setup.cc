/*
 * txtio_setup.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */
#include "../txtio_imp.h"
#include <txtio/txtio_setup.hh>
#include <txtio/txtio_mutex.hh>
#include "txtio/inout.h"

#include "driver/uart.h"




#include <uout/uo_callbacks.h>

// sys
//#include "esp_console.h"
#include "esp_vfs_dev.h"
#include <esp_system.h>
#include <esp32/rom/ets_sys.h>
#include "esp_log.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

static const int UART_RX_RINGBUF_SIZE = (1024 * 2);
static const int UART_TX_RINGBUF_SIZE = (1024 * 1);
static const uart_port_t UART_NUM = static_cast<uart_port_t>(CONFIG_ESP_CONSOLE_UART_NUM);

typedef int (*writeFn)(const char *, size_t len);



static int my_uart_write(const char *src, size_t len) {
  return uart_write_bytes(UART_NUM, src, len);
}

static int priv_write(const char *s, size_t len, writeFn wfn) {
  const size_t size = len;
  const char crlf[] = "\r\n";
  const unsigned crlf_len = sizeof crlf - 1;
  char prev_c = 0;
  int last_chunk = 0;

  for (size_t i = 0; i < size; ++i) {
    const char c = s[i];
    if (c == '\r' || (c == '\n' && prev_c == '\r'))
      continue;

    if (c == '\n') {
      const int chunk_len = i - last_chunk;
      if (chunk_len > 0 && wfn(s + last_chunk, chunk_len) < 1)
        return -1;
      last_chunk = i + 1;
      if (wfn(crlf, crlf_len) < 1)
        return -1;
    } else if (i + 1 == size) {
      const int chunk_len = i - last_chunk + 1;
      if (chunk_len > 0 && wfn(s + last_chunk, chunk_len) < 1)
        return -1;
      break;
    }
    prev_c = c;
  }

  return len;
}


static int my_write(const char *src, size_t len) {
  return priv_write(src, len, my_uart_write);
}



static int  es_io_getc(void) {
#ifdef CONFIG_APP_USE_CLI_TASK
 return getchar();
#else
 char c;
 if (read(STDIN_FILENO, &c, 1) == 1)
   return c;

 return -1;
#endif
}


static int es_io_putc(char c) {
#ifdef CONFIG_APP_USE_CLI_TASK
  return my_write(&c, 1);
#else
  return putchar(c);
#endif
}



static int es_io_putc_crlf(char c) {
  if (c == '\r')
    return 1;
  if (c == '\n') {
    if (es_io_putc('\r') < 0)
      return -1;
  }
  return es_io_putc(c);
}


static void initialize_console(struct cfg_txtio *cfg_txtio)
{
  static bool once;
  if (once)
    return;
  once = 1;

    /* Drain stdout before reconfiguring it */
    fflush(stdout);
    fsync(fileno(stdout));

    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
  //  esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
 //   esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    /* Configure UART. Note that REF_TICK is used so that the baud rate remains
     * correct while APB frequency is changing in light sleep mode.
     */
    const uart_config_t uart_config = {
            .baud_rate = cfg_txtio->baud,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .source_clk = UART_SCLK_REF_TICK,
           //.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
           // .rx_flow_ctrl_thresh = 64,
    };
    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install(UART_NUM, UART_RX_RINGBUF_SIZE, UART_TX_RINGBUF_SIZE, 0, NULL, 0) );
    ESP_ERROR_CHECK( uart_param_config(UART_NUM, &uart_config) );

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(UART_NUM);
#if 0
    /* Initialize the console */
    esp_console_config_t console_config = {
            .max_cmdline_args = 8,
            .max_cmdline_length = 256,
#if CONFIG_LOG_COLORS
            .hint_color = atoi(LOG_COLOR_CYAN)
#endif
    };
    ESP_ERROR_CHECK( esp_console_init(&console_config) );

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within
     * single line.
     */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

    /* Set command history size */
    linenoiseHistorySetMaxLen(100);

    /* Don't return empty lines */
    linenoiseAllowEmpty(false);

#if CONFIG_STORE_HISTORY
    /* Load command history from filesystem */
    linenoiseHistoryLoad(HISTORY_PATH);
#endif
#endif
}

static void pctChange_cb(const uoCb_msgT msg) {
  LockGuard lock(txtio_mutex);

  if (auto txt = uoCb_txtFromMsg(msg)) {
#ifdef CONFIG_APP_USE_CLI_TASK
    uart_write_bytes(UART_NUM, txt, strlen(txt));
    uart_write_bytes(UART_NUM, "\r\n", 2);
#else
  ::write(1, txt, strlen(txt));
  ::write(1, "\r\n", 2);
#endif
  }
  if (auto json = uoCb_jsonFromMsg(msg)) {
#ifdef CONFIG_APP_USE_CLI_TASK
    uart_write_bytes(UART_NUM, json, strlen(json));
    uart_write_bytes(UART_NUM, "\r\n", 2);
#else
  ::write(1, json, strlen(json));
  ::write(1, "\r\n", 2);
#endif
  }
}

static void callback_subscribe(uo_flagsT flags) {
  flags.evt.pin_change = true;
  flags.evt.ip_address_change = true;
  flags.evt.gen_app_state_change = true;
  flags.evt.gen_app_error_message = true;
  flags.evt.gen_app_log_message = true;
  flags.fmt.json = true;
  flags.fmt.txt = true;
  uoCb_subscribe(pctChange_cb, flags);
}


extern "C++" void txtio_mcu_setup();

void txtio_mcu_setup(struct cfg_txtio *cfg_txtio) {
#ifdef CONFIG_APP_USE_CLI_TASK
  initialize_console(cfg_txtio);
#endif
  io_putc_fun = es_io_putc_crlf;
  io_getc_fun = es_io_getc;
  con_printf_fun = ets_printf;

  esp_log_level_set("*", static_cast<esp_log_level_t>(cfg_txtio->verbose));
#ifndef DISTRIBUTION
  //esp_log_level_set("wifi", ESP_LOG_ERROR);
#endif
#if 0
  if (TXTIO_IS_VERBOSE(6)) {
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
  }
#endif

  callback_subscribe(cfg_txtio->flags);
}
