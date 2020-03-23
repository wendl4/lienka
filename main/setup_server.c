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
        httpd_resp_send_chunk(req, buf, ret);
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
    httpd_resp_send_chunk(req, NULL, 0);
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

    }

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
    httpd_resp_send_chunk(req, NULL, 0);
    esp_restart();
    return ESP_OK;
}

static const httpd_uri_t reset_nvs = {
    .uri       = "/reset-nvs",
    .method    = HTTP_POST,
    .handler   = reset_nvs_post_handler,
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