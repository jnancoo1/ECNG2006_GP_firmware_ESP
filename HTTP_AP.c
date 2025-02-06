/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "driver/gpio.h"
#include "lwip/err.h"
#include "lwip/sys.h"

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      "CONFIG_ESP_WIFI_SSID"
#define EXAMPLE_ESP_WIFI_PASS      "CONFIG_ESP_WIFI_PASSWORD"
#define EXAMPLE_MAX_STA_CONN       10
#define UART_NUM                   UART_NUM_0
#define BUF_SIZE                   1024


static const char *TAG = "wifi softAP";
const char *to_print="This is the first Messagge You will See \n";
int a=10;

const char *button_message="a";

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}
int b=500;
void wifi_init_softap()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

esp_err_t root_get_handler(httpd_req_t *req)
{
    const char *response = "<!DOCTYPE html>"
                       "<html>"
                       "<head><title>ECNG 2006 Group Project Reporting</title></head>"
                       "<script>"

                       "function fetchEvent() {"
  "fetch('/event')"  // Send an HTTP GET request to the "/event" endpoint
    ".then(response => response.json()) " // Parse the response as JSON
    ".then(data => {"  
      // Access the event and sensor data from the JSON response
      "document.getElementById('sensor-data').innerText = 'Sensor Data:' + data.sensor_data;"
    "})"
    ".catch(error => console.error('Error fetching event:', error));"  // Handle errors
"}"

// Call the `fetchEvent` function every 5 seconds (5000 milliseconds)
"setInterval(fetchEvent, 5000);"
                       "</script>"
                       "</head>"
                       "<body>"
                       "<h1>Good day</h1>"
                       "<p>Your uploaded sensor readings will be displayed .</p>"
                       "<div id='sensor-data'>This will disappear on successful upload</div>"
                       "</body>"
                       "</html>";

    httpd_resp_send(req, response, HTTPD_MAX_URI_LEN);
    return ESP_OK;
}

esp_err_t event_get_handler(httpd_req_t *req)
{
    
    ESP_LOGI(TAG, "Event");

    char event_data[100];
    snprintf(event_data, sizeof(event_data), "{\"event\": \"New sensor data available!\", \"sensor_data\": \"%s\"}", button_message);
    
    ESP_LOGI(TAG, "UPDATED");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    httpd_resp_send(req, event_data, strlen(event_data));
    ESP_LOGI(TAG, "Done");

    return ESP_OK;
}

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(TAG, "Starting HTTP server");
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = root_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &root_uri);

        httpd_uri_t event_uri = {
        .uri       = "/event",
        .method    = HTTP_GET,
        .handler   = event_get_handler,
        .user_ctx  = NULL
};
httpd_register_uri_handler(server, &event_uri);

    } else {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }
    return server;
}


void app_main()
{


      uart_config_t uart_config = {
        .baud_rate = 1200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);

    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
    uint8_t buffer[BUF_SIZE]; // Buffer to store extracted message
    int buffer_index = 0;
    bool recording = false; 


    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();

  start_webserver();

    gpio_config_t config1;
    config1.mode=GPIO_MODE_DEF_INPUT;
    config1.pin_bit_mask=(1ULL<<GPIO_NUM_0);
    config1.intr_type = GPIO_INTR_DISABLE;
    config1.pull_down_en = 0;
    config1.pull_down_en = 0;
    gpio_config(&config1);
    int f=0;
    while(1){
        f=gpio_get_level(GPIO_NUM_0);
        free(buffer);
        if(f==0){
            a+=10;
            button_message="The Button Is Pressed";
            printf("here\n");
            vTaskDelay(10000 / portTICK_PERIOD_MS);

        }
        else{
            printf("here2 \n");

            a+=10;
            button_message="The Button Is Not Pressed";
            printf("None\n");
            vTaskDelay(10000 / portTICK_PERIOD_MS);
        }



        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE - 1, 100 / portTICK_PERIOD_MS);
        if (len > 0) {
            data[len] = '\0'; // Null-terminate received data

            for (int i = 0; i < len; i++) {
                if (!recording) {
                    // Detect "aa" start sequence
                    if (i < len - 1 && data[i] == 'a' && data[i + 1] == 'a') {
                        recording = true;
                        buffer_index = 0;
                        i++; // Skip the next 'a'
                        continue;
                    }
                } else {
                    // Detect "xx" end sequence
                    if (i < len - 1 && data[i] == 'x' && data[i + 1] == 'x') {
                        buffer[buffer_index] = '\0'; // Null-terminate buffer
                        printf("Extracted Message: %s\n", buffer);
                        recording = false;
                        buffer_index = 0;
                        i++; // Skip the next 'x'
                        continue;
                    }

                    // Store character in buffer
                    if (buffer_index < BUF_SIZE - 1) {
                        buffer[buffer_index++] = data[i];
                    }
                }
            }
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    free(data);

    }  
