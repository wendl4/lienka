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


nvs_handle my_handle;
esp_err_t ret;

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
    char *mdns_name = malloc(64);
    size_t mdns_name_size = 64;

    // get name from NVS
    nvs_open("storage", NVS_READWRITE, &my_handle);
    nvs_get_str(my_handle, "mdns_name", mdns_name, &mdns_name_size);

    ESP_ERROR_CHECK(mdns_hostname_set(mdns_name));
    ESP_LOGI(TAG, "mdns hostname set to: [%s]", mdns_name);
    //set default instance
    ESP_ERROR_CHECK( mdns_instance_name_set("Beebot Mdns") );
    ESP_ERROR_CHECK( mdns_service_add(NULL, "_http", "_udp", 80, NULL, 0) );
}

// NVS CLEAR

void clearNVS()
{

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
