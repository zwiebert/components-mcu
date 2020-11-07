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



#include "app_config/proj_app_cfg.h"
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


#define UART_RX_RINGBUF_SIZE (1024 * 2)
#define UART_TX_RINGBUF_SIZE (1024 * 1)

static int  es_io_getc(void) {
#ifdef USE_CLI_TASK
 return getchar();
#else
 char c;
 if (read(STDIN_FILENO, &c, 1) == 1)
   return c;

 return -1;
#endif
}

static int es_io_putc(char c) {

#ifdef USE_CLI_TASK
  return uart_write_bytes(CONFIG_ESP_CONSOLE_UART_NUM, &c, 1);
#else
  return putchar(c);
#endif
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
    ESP_ERROR_CHECK( uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, UART_RX_RINGBUF_SIZE, UART_TX_RINGBUF_SIZE, 0, NULL, 0) );
    ESP_ERROR_CHECK( uart_param_config(CONFIG_ESP_CONSOLE_UART_NUM, &uart_config) );

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
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
#ifdef USE_CLI_TASK
  uart_write_bytes(CONFIG_ESP_CONSOLE_UART_NUM, txt, strlen(txt));
#else
  ::write(1, txt, strlen(txt));
#endif
  }
  if (auto json = uoCb_jsonFromMsg(msg)) {
#ifdef USE_CLI_TASK
  uart_write_bytes(CONFIG_ESP_CONSOLE_UART_NUM, json, strlen(json));
  uart_write_bytes(CONFIG_ESP_CONSOLE_UART_NUM, ";\n", 2);
#else
  ::write(1, json, strlen(json));
#endif
  }
}

static void callback_subscribe(uo_flagsT flags) {
  flags.evt.pin_change = true;
  flags.evt.pct_change = true;
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
#ifdef USE_CLI_TASK
  initialize_console(cfg_txtio);
#endif
  io_putc_fun = es_io_putc;
  io_getc_fun = es_io_getc;
  con_printf_fun = ets_printf;

  esp_log_level_set("*", static_cast<esp_log_level_t>(cfg_txtio->verbose));
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
