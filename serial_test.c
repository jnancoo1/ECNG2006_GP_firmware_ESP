
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "driver/uart.h"
#include "lwip/err.h"
#include "lwip/sys.h"

void app_main(){


    uart_config_t configx;
    configx.baud_rate=1200;
    configx.data_bits = UART_DATA_8_BITS;
    configx.parity=UART_PARITY_DISABLE;
    configx.stop_bits = UART_STOP_BITS_1;
    configx.flow_ctrl=UART_HW_FLOWCTRL_DISABLE;
    uart_driver_install(UART_NUM_0, 1024 * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &configx);

    uint8_t *data = (uint8_t *) malloc(1024);

    while (1) {
    int len = uart_read_bytes(UART_NUM_0, data, 1024, 100 / portTICK_RATE_MS);

     if (len > 0) {
            data[len] = '\0'; // Null-terminate the received data
            printf("Received: %s\n", (char *)data);
        }
    len=0;

    vTaskDelay(20000 / portTICK_RATE_MS);
    printf("Test Loop\n");

}


}
