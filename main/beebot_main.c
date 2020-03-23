#include <string.h>
// #include "stepper.h"
#include <stdio.h>
#include "nvs_flash.h"
#include "setup_ap.h"
#include "setup_sta.h"
#include "setup_server.h"
#include "other_functionality.h"



void app_main()
{
    /*** init variables ***/
    int is_set = 0;
    char *ssid = malloc(32);
    size_t size = 32;
    char *password = malloc(64);
    size_t p_size = 64;


    // NVS INIT START
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    // NVS INIT END

    nvs_handle my_handle;
    ret = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (ret != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
    }

    else
    {
        // FIND OUT IF SSID AND PASSWORD IS SET START
        nvs_get_str(my_handle, "password", password, &p_size);
        ret = nvs_get_str(my_handle, "ssid", ssid, &size);
        switch (ret) {
            case ESP_OK:
                printf("Done\n");
                printf("SSID = %s, password = %s \n", ssid,password);
                is_set = 1;
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(ret));
        }
        // FIND OUT IF SSID AND PASSWORD IS SET END
    }


    // AP/STA SETUP START
   if (is_set == 0)
    {
        setup_ap_mode();
    }
    else if (is_set == 1)
    {
        setup_sta_mode(ssid,password);
    }
    else if (is_set == 2)
    {
        nvs_erase_all(my_handle);
        nvs_commit(my_handle);
    }
    start_mdns_service();
    // AP/STA SETUP END

    // SERVER SETUP
    setup_server();
}
