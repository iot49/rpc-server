/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// Sample at
// ~/.platformio/packages/framework-espidf/examples/wifi/getting_started/station

#include "network.h"
#include "error.h"

#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/apps/sntp.h"

#define MIN(a,b) ( (a) < (b) )? (a) : (b)

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event 
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "network";

static void start_sntp()
{
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    Network *network = (Network*)ctx;
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
    {
        String ip = ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip);
        network->status_msg = "connected";
        network->connected = true;
        network->ip_addr = ip;
        ESP_LOGI(TAG, "got ip: %s", ip.c_str());
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        if (!network->sntp_started) {
            start_sntp();
            network->sntp_started = true;
        }
        break;
    }
    case SYSTEM_EVENT_STA_DISCONNECTED:
    {
        network->connected = false;
        system_event_sta_disconnected_t *disconn = &event->event_info.disconnected;
        switch (disconn->reason)
        {
        case WIFI_REASON_BEACON_TIMEOUT:
            // AP has dropped out; try to reconnect.
            network->status_msg = "AP beacon timeout";
            break;
        case WIFI_REASON_NO_AP_FOUND:
            // AP may not exist, or it may have momentarily dropped out; try to reconnect.
            network->status_msg = "no AP found";
            break;
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
        case WIFI_REASON_AUTH_FAIL:
            // Password may be wrong, or it just failed to connect; try to reconnect.
            network->status_msg = "authentication failed";
            break;
        default:
            // Let other errors through and try to reconnect.
            network->status_msg = "disconnected for unspecified reason";
            break;
        }
        ESP_LOGI(TAG, "STA_DISCONNECTED, reason:%d - %s", disconn->reason, network->status_msg.c_str());

        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "trying to reconnect to WiFi AP");
        break;
    }
    default:
        break;
    }
    return ESP_OK;
}

void Network::connect(String ssid, String password)
{
    if (sta_initialized) {
        ESP_LOGI(TAG, "call connect only once (ignored)");
        return;
    }
    sta_initialized = true;

    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    err_check_throw(esp_event_loop_init(event_handler, this));

    ESP_LOGI(TAG, "Inititalizing WiFi");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err_check_throw(esp_wifi_init(&cfg));
    err_check_throw(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_LOGI(TAG, "WiFi initialized");

    err_check_throw(esp_wifi_set_mode(WIFI_MODE_STA));

    // Allocate storage for the struct
    wifi_config_t wifi_config {};

    memcpy(wifi_config.sta.ssid, ssid.c_str(), MIN(ssid.length(), sizeof(wifi_config.sta.ssid)));
    memcpy(wifi_config.sta.password, password.c_str(), MIN(password.length(), sizeof(wifi_config.sta.password)));

    err_check_throw(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    err_check_throw(esp_wifi_start());

    status_msg = "connecting";
    ESP_LOGI(TAG, "WiFi connecting to ap SSID: %s", ssid.c_str());
}

int Network::epoch()
{
    time_t now;
    time(&now);
    return now;
}
