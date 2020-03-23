#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <mdns.h>

// MDNS START

void start_mdns_service()
{
    static const char *TAG = "mdns";
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) {
        printf("MDNS Init failed: %d\n", err);
        return;
    }

    //set hostname
    char *hostname = "lienka";
    ESP_ERROR_CHECK( mdns_hostname_set(hostname));
    ESP_LOGI(TAG, "mdns hostname set to: [%s]", hostname);
    //set default instance
    ESP_ERROR_CHECK( mdns_instance_name_set("Beebot Mdns") );
    ESP_ERROR_CHECK( mdns_service_add(NULL, "_http", "_udp", 80, NULL, 0) );
}

// NVS CLEAR

void clearNVS()
{
    nvs_handle my_handle;
    esp_err_t ret;
    ret = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (ret != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
    }

    else
    {
        nvs_erase_all(my_handle);
        nvs_commit(my_handle);
    }
}
