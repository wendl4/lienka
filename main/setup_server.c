#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include <esp_http_server.h>

#include <unistd.h>
#include <mdns.h>

#define MAX_STRING_LEN 64


static const char *TAG = "server";

/* Find parameter value in query */
char* findArg(char* arg, char* string) {
    char* mystring = strdup(string);
    char output[MAX_STRING_LEN] = "";
    char* argtemplate;
    argtemplate = strdup(arg);
    char token[MAX_STRING_LEN] = "";
    while (*mystring != '\0') {
        if (*argtemplate == *mystring && strlen(token) < 64) {
            if (strlen(token) > MAX_STRING_LEN) {
                ESP_LOGI(TAG,"WARNING: Arg name longer than 64 chars, you need to make it shorter");
                return NULL;
            }
            token[strlen(token)] = *mystring;
            argtemplate++;
            if (strcmp(token,arg) == 0) {
                mystring++;
                if (*mystring == '=') {
                    mystring++;
                    while(*mystring != '&' && *mystring != '\0') {
                        if (strlen(output) > MAX_STRING_LEN) {
                            ESP_LOGI(TAG,"WARNING: Value longer than 64 chars, you need to make it shorter");
                            return NULL;
                        }
                        output[strlen(output)] = *mystring;
                        mystring++;
                    }
                    return strdup(output);
                }
                argtemplate = strdup(arg);
                memset(token, 0, strlen(arg)+1);            
            } 
        }
        mystring++;
    }
    return strdup(output);
}









/* HTTP POST handlers */
static esp_err_t sendsteps_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;
    httpd_resp_set_hdr(req,"Access-Control-Allow-Origin","*");

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t sendsteps = {
    .uri       = "/sendsteps",
    .method    = HTTP_POST,
    .handler   = sendsteps_post_handler,
    .user_ctx  = NULL
};











/********** setup-sta handler ***********/

static esp_err_t setup_sta_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;
    httpd_resp_set_hdr(req,"Access-Control-Allow-Origin","*");

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        
        remaining -= ret;

        /* Log data received */
        char* mybuf = strndup(buf,ret);
        char* ssid = findArg("ssid",mybuf);
        char* pass = findArg("pass",mybuf);

        /* Write sta config to NVS */
        nvs_handle my_handle;
        esp_err_t ret;
        ret = nvs_open("storage", NVS_READWRITE, &my_handle);
        if (ret != ESP_OK) {
            printf("Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
        }
        else {
            ret = nvs_set_str(my_handle, "ssid", ssid);
            ret = nvs_set_str(my_handle, "password", pass);
        }

        free(ssid);
        free(pass);
    }

    // End response
    httpd_resp_send(req, NULL, 0);
    // need to wait for response to send
    sleep(1);
    esp_restart();
    return ESP_OK;
}

static const httpd_uri_t setup_sta = {
    .uri       = "/setup-sta",
    .method    = HTTP_POST,
    .handler   = setup_sta_post_handler,
    .user_ctx  = NULL
};









/********** reset-nvs handler ***********/

static esp_err_t reset_nvs_post_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr(req,"Access-Control-Allow-Origin","*");

	nvs_handle my_handle;
    esp_err_t re;
    re = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (re != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(re));
    }

    else
    {
		nvs_erase_all(my_handle);
    	nvs_commit(my_handle);
	}

    // End response
    httpd_resp_send(req, NULL, 0);
    // need to wait for response to send
    sleep(1);
    esp_restart();
    return ESP_OK;
}

static const httpd_uri_t reset_nvs = {
    .uri       = "/reset-nvs",
    .method    = HTTP_POST,
    .handler   = reset_nvs_post_handler,
    .user_ctx  = NULL
};







/********** status handler ***********/

static esp_err_t status_get_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr(req,"Access-Control-Allow-Origin","*");
    /* Send a simple response */
    wifi_mode_t mode;
    char *resp;
    esp_wifi_get_mode(&mode);
    if (mode == WIFI_MODE_STA) {
        resp = "STA";
    }
    else {
        resp = "AP";
    }
    httpd_resp_send(req, resp, strlen(resp));
    return ESP_OK;
}

static const httpd_uri_t get_status = {
    .uri       = "/status",
    .method    = HTTP_GET,
    .handler   = status_get_handler,
    .user_ctx  = NULL
};





/********** mdns get handler ***********/

static esp_err_t hostname_change_post_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr(req,"Access-Control-Allow-Origin","*");

    /* set mdns name to NVS */
    char buf[64];

    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(buf));

    httpd_req_recv(req, buf, recv_size);
    ESP_LOGI(TAG, "New hostname is: %s \n",buf);

    nvs_handle my_handle;
    nvs_open("storage", NVS_READWRITE, &my_handle);
    nvs_set_str(my_handle, "mdns_name", buf);

    ESP_ERROR_CHECK(mdns_hostname_set(buf));
    ESP_LOGI(TAG, "mdns hostname set to: [%s]", buf);

    httpd_resp_send(req, buf, strlen(buf));

    return ESP_OK;
}

static const httpd_uri_t get_mdns_name = {
    .uri       = "/change-hostname",
    .method    = HTTP_POST,
    .handler   = hostname_change_post_handler,
    .user_ctx  = NULL
};








static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &sendsteps);
        httpd_register_uri_handler(server, &setup_sta);
        httpd_register_uri_handler(server, &reset_nvs);
        httpd_register_uri_handler(server, &get_status);
        httpd_register_uri_handler(server, &get_mdns_name);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void setup_server() {
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());

    /* Start the server for the first time */
    server = start_webserver();
}