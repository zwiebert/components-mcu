#include "stm32/esp32/stm32_uart.hh"
#include "stm32/lock_guard.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "debug/dbg.h"
#include "utils_misc/int_types.h"
#include <stdio.h>
#include <stdint.h>

#ifdef CONFIG_STM32_DEBUG
#define DEBUG
#define D(x) x
#define DD(x)
#else
#define D(x)
#define DD(x)
#endif
#define logtag "stm32"
#define TAG logtag

#define stm32_cfg (&stm32_config)
#define STM32_SET_BOOT_PIN(level) gpio_set_level((gpio_num_t)stm32_cfg->boot_gpio, (!level != !stm32_cfg->boot_gpio_is_inverse));
#define RX_BUF_SIZE 2048
#define TX_BUF_SIZE 0
#define RESET_PIN_MS 1000
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

static const char PATTERN_CHR = '\n';
static const uint8_t PATTERN_CHR_NUM = 1;
static const uint8_t PATTERN_NUM_MAX = 20;

bool Stm32_Uart_ESP32::stm32_isFirmwareRunning(void) {
  return stm32_mode == STM32_MODE_FIRMWARE;
}

void Stm32_Uart_ESP32::stm32_reset() {
  LockGuard lock(stm32_mutex);
  ESP_LOGI(TAG, "reboot stm32");
  gpio_set_level((gpio_num_t) stm32_cfg->reset_gpio, 0);
  vTaskDelay(RESET_PIN_MS / portTICK_PERIOD_MS);
  gpio_set_level((gpio_num_t) stm32_cfg->reset_gpio, 1);
}

int Stm32_Uart_ESP32::p_stm32_write(const char *data, unsigned data_len) {
  //LockGuard lock(stm32_mutex);
  if (stm32_mode != STM32_MODE_FIRMWARE)
    return -1;
  D(ESP_LOGI(logtag,  "%s: dlen=%u, data=<%.*s>", __func__, data_len, data_len, data));
  return uart_write_bytes(m_uart, (const char*) data, data_len);
}

int Stm32_Uart_ESP32::p_stm32_read(char *buf, unsigned buf_size) {
  //LockGuard lock(stm32_mutex);
  if (stm32_mode != STM32_MODE_FIRMWARE)
    return -1;
  return uart_read_bytes(m_uart, (uint8_t*) buf, buf_size, 20 / portTICK_PERIOD_MS);
}

void Stm32_Uart_ESP32::stm32_wake_up_queue() {
  if (!uart0_queue)
    return;

  uart_event_t event = { .type = (uart_event_type_t) MY_UART_WAKE_UP };
  xQueueSend(uart0_queue, &event, 20 / portTICK_PERIOD_MS);
}

int Stm32_Uart_ESP32::p_stm32_read_line(char *buf, unsigned buf_size, unsigned wait_ms) { // XXX: Implement parameter wait_ms, which is currently ignored
  uart_event_t event;
  uint8_t *dtmp = (uint8_t*) buf;

  while (uart0_queue) {

    {
      LockGuard lock(stm32_mutex);
      //Waiting for UART event.
      if (!xQueueReceive(uart0_queue, (void*) &event, (TickType_t) 1000 / portTICK_PERIOD_MS))
        continue;

      DD(ESP_LOGI(TAG, "uart[%d] event:", m_uart));
      if (event.type == (uart_event_type_t) MY_UART_WAKE_UP)
        return 0;

      switch (event.type) {

      //Event of UART receving data
      /*We'd better handler data event fast, there would be much more data events than
       other types of events. If we take too much time on data event, the queue might
       be full.*/
      case UART_DATA: {
        //LockGuard lock(stm32_mutex);

        DD(ESP_LOGI(TAG, "[UART DATA]: %d", event.size));
        uart_read_bytes(m_uart, dtmp, event.size, portMAX_DELAY);
        DD(ESP_LOGI(TAG, "[DATA EVT]:"));
        uart_write_bytes(m_uart, (const char*) dtmp, event.size);
      }
        break;

        //Event of HW FIFO overflow detected
      case UART_FIFO_OVF: {
        //LockGuard lock(stm32_mutex);
        D(ESP_LOGI(TAG, "hw fifo overflow"));
        // If fifo overflow happened, you should consider adding flow control for your application.
        // The ISR has already reset the rx FIFO,
        // As an example, we directly flush the rx buffer here in order to read more data.
        uart_flush_input(m_uart);
        xQueueReset(uart0_queue);
      }
        break;

        //Event of UART RX break detected
      case UART_BREAK:
        D(ESP_LOGI(TAG, "uart rx break"));
        break;

        //Event of UART parity check error
      case UART_PARITY_ERR:
        D(ESP_LOGI(TAG, "uart parity error"));
        break;

        //Event of UART frame error
      case UART_FRAME_ERR:
        D(D(ESP_LOGI(TAG, "uart frame error")));
        break;

        //Event of UART ring buffer full
      case UART_BUFFER_FULL: {
        //LockGuard lock(stm32_mutex);
        D(ESP_LOGI(TAG, "ring buffer full"));
        int result = uart_read_bytes(m_uart, (uint8_t*) buf, buf_size, 100 / portTICK_PERIOD_MS);
        xQueueReset(uart0_queue);
        return result;
      }
        break;

        //UART_PATTERN_DET
      case UART_PATTERN_DET: {
        //LockGuard lock(stm32_mutex);
        size_t buffered_size;
        uart_get_buffered_data_len(m_uart, &buffered_size);
        int pos = uart_pattern_pop_pos(m_uart);
        DD(ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size));
        if (pos == -1) {
          // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
          // record the position. We should set a larger queue size.
          // As an example, we directly flush the rx buffer here.
          uart_flush_input(m_uart);
        } else {
          int result = uart_read_bytes(m_uart, dtmp, pos + PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
          return result;  // return with PATTERN_CHR (LF) so the caller can distinguish between incomplete lines from UART_BUFFER_FULL
        }
      }
        break;

        //Others
      default:
        ESP_LOGI(TAG, "uart event type: %d", event.type);
        break;
      }
    }
  }
  return -1;
}

int Stm32_Uart_ESP32::p_stm32_getc(bool block) {
  //LockGuard lock(stm32_mutex);
  if (stm32_mode != STM32_MODE_FIRMWARE)
    return -1;

  uint8_t buf = 0;
  if (uart_read_bytes(m_uart, (uint8_t*) &buf, 1, block ? (20 / portTICK_PERIOD_MS) : 1) == 1)
    return buf;

  return -1;
}
int Stm32_Uart_ESP32::p_stm32_write_bl(const char *data, unsigned data_len) {
  //LockGuard lock(stm32_mutex);
  if (stm32_mode != STM32_MODE_BOOTLOADER)
    return -1;
  return uart_write_bytes(m_uart, (const char*) data, data_len);
}

int Stm32_Uart_ESP32::p_stm32_read_bl(char *buf, unsigned buf_size) {
  //LockGuard lock(stm32_mutex);
  if (stm32_mode != STM32_MODE_BOOTLOADER)
    return -1;
  return uart_read_bytes(m_uart, (uint8_t*) buf, buf_size, 20 / portTICK_PERIOD_MS);
}

void Stm32_Uart_ESP32::stm32_configSerial(stm32_mode_T mode) {
  WakeUpLockGuard lock(stm32_mutex);

  esp_err_t err;
  if (mode == stm32_mode) {
    D(ESP_LOGI(TAG,"Keep configuration and mode unchanged (mode=%d)", stm32_mode));
    return; // do nothing if mode stays the same
  }

// If a driver was installed then we need to delete it before doing anything
  if (stm32_mode != STM32_MODE_NONE) {
    D(ESP_LOGI(TAG,"Delete UART driver (old_mode=%d)", stm32_mode));
    uart0_queue = nullptr;
    uart_driver_delete(m_uart);
    stm32_mode = STM32_MODE_NONE;
  }

// If the request was to uninstall the driver, we are done.
  if (mode == STM32_MODE_NONE) {
    D(ESP_LOGI(TAG,"UART-driver uninstalled, as requested. (old_mode=%d)", stm32_mode));
    stm32_mode = mode;
    return;
  }

  D(ESP_LOGI(TAG,"Setup UART driver (new_mode=%d)", mode));
// From here on: Setup and install the driver

  /* Configure parameters of an UART driver,
   * communication pins and install the driver */
  uart_config_t uart_config = { .baud_rate = 115200, .data_bits = UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits = UART_STOP_BITS_1, .flow_ctrl =
      UART_HW_FLOWCTRL_DISABLE, .source_clk = UART_SCLK_DEFAULT };

// The stm32 boot loader needs even parity
  if (mode == STM32_MODE_BOOTLOADER) {
    uart_config.parity = UART_PARITY_EVEN;
    D(ESP_LOGI(TAG, "uart1: even parity\n"));
  }

  if ((err = uart_param_config(m_uart, &uart_config))) {
    ESP_LOGE(TAG, "uart_param_config failed: %s", esp_err_to_name(err));
    return;
  }

  if ((err = uart_set_pin(m_uart, stm32_cfg->uart_tx_gpio, stm32_cfg->uart_rx_gpio, ECHO_TEST_RTS, ECHO_TEST_CTS))) {
    ESP_LOGE(TAG, "uart_set_pin failed: %s", esp_err_to_name(err));
    return;
  }

  auto queue = (mode == STM32_MODE_FIRMWARE) ? &uart0_queue : nullptr;
  auto queue_size = (mode == STM32_MODE_FIRMWARE) ? PATTERN_NUM_MAX : 0;

  if ((err = uart_driver_install(m_uart, RX_BUF_SIZE, TX_BUF_SIZE, queue_size, queue, 0))) {
    ESP_LOGE(TAG, "uart_driver_install failed: %s", esp_err_to_name(err));
    return;
  }

  if (mode == STM32_MODE_FIRMWARE) {
    //Set uart pattern detect function.
    uart_enable_pattern_det_baud_intr(m_uart, PATTERN_CHR, PATTERN_CHR_NUM, 9, 0, 0);
    //Reset the pattern queue length to record at most PATTERN_NUM_MAX pattern positions.
    uart_pattern_queue_reset(m_uart, PATTERN_NUM_MAX);
  }
  ESP_LOGI(TAG, "uart driver succesfully installed");
  stm32_mode = mode;
}

void Stm32_Uart_ESP32::stm32_runBootLoader() {
  WakeUpLockGuard lock(stm32_mutex);
  STM32_SET_BOOT_PIN(1);
  stm32_reset();
  stm32_configSerial(STM32_MODE_BOOTLOADER);
}

void Stm32_Uart_ESP32::stm32_runFirmware() {
  WakeUpLockGuard lock(stm32_mutex);
  STM32_SET_BOOT_PIN(0);
  stm32_reset();
  stm32_configSerial(STM32_MODE_FIRMWARE);
}

void Stm32_Uart_ESP32::stm32_setup(const cfg_stm32 *cfg) {
  WakeUpLockGuard lock(stm32_mutex);
  stm32_config = *cfg;

  gpio_set_direction((gpio_num_t) cfg->reset_gpio, GPIO_MODE_OUTPUT_OD);
  gpio_set_level((gpio_num_t) cfg->reset_gpio, 1);

  gpio_set_direction((gpio_num_t) cfg->boot_gpio, cfg->boot_gpio_is_inverse ? GPIO_MODE_OUTPUT_OD : GPIO_MODE_OUTPUT); // XXX: if inverse we would have a pullup resistor (guesswork)
  STM32_SET_BOOT_PIN(0);

  stm32_configSerial(STM32_MODE_FIRMWARE);
}
