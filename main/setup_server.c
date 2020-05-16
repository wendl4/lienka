#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.h"
#include "esp_camera.h"
#include "qr_recognition.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
#include "perform.h"
#include "file_server.h"

#define MAX_STRING_LEN 64

static const char *TAG = "server";

#define BUFFER_LEN 512
/* Convert the pgm gray in a rgb bitmap */
static esp_err_t make_gray_bmp(httpd_req_t *req, camera_fb_t * fb)
{
    char* buf;
    int x = 0;
    int size;
    esp_err_t err = ESP_OK;

    if (!fb) {
        ESP_LOGE(TAG, "Camera Capture Failed");
        return ESP_FAIL;
    }

    /* To save RAM send the converted image in chunks of 512 bytes. */
    buf = malloc( BUFFER_LEN * 3 );

    if (!buf) {
        ESP_LOGE(TAG, "Dynamic memory failed");
        return ESP_FAIL;
    }

    while ( (x<fb->len) && (err == ESP_OK) ) {
        size = (fb->len >= BUFFER_LEN) ? BUFFER_LEN : fb->len;

        /* To convert, match the RGB bytes to the value of the PGM byte. */
        for (int i=0; i<size; i++) {
            buf[i * 3 ] = fb->buf[i + x];
            buf[(i * 3) + 1 ] = fb->buf[i + x];
            buf[(i * 3) + 2 ] = fb->buf[i + x];
        }

        err = httpd_resp_send_chunk(req, buf, size * 3);
        x += size;
    }

    free(buf);

    return err;
}

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
    char buf[256];
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

        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "steps");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        char* code = strndup(buf,ret);
        prepair_program(code);
    }

    // End response
    httpd_resp_send(req, NULL, 0);
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
        char* arguments = strndup(buf,ret);
        char* ssid = findArg("ssid",arguments);
        char* pass = findArg("pass",arguments);

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


static const httpd_uri_t mdns_name = {
    .uri       = "/change-hostname",
    .method    = HTTP_POST,
    .handler   = hostname_change_post_handler,
    .user_ctx  = NULL
};



/* HTTP bmp handler to take one picture*/
static esp_err_t handle_rgb_bmp(httpd_req_t *req)
{
    esp_err_t err = ESP_OK;
    httpd_resp_set_hdr(req,"Access-Control-Allow-Origin","*");

    camera_fb_t * fb = esp_camera_fb_get();

    sensor_t * sensor = esp_camera_sensor_get();

    if (!fb) {
        ESP_LOGE(TAG, "Camera Capture Failed");
        return ESP_FAIL;
    }

    bitmap_header_t* header = bmp_create_header(fb->width, fb->height);
    if (header == NULL) {
        return ESP_FAIL;
    }

    err = httpd_resp_set_type(req, "image/bmp");

    if (err == ESP_OK){
        err = httpd_resp_set_hdr(req, "Content-disposition", "inline; filename=capture.bmp");
    }

    if (err == ESP_OK) {
        err = httpd_resp_send_chunk(req, (const char*)header, sizeof(*header));
    }

    free(header);

    if (err == ESP_OK) {
        /* convert an image with a gray format of 8 bits to a 24 bit bmp. */
        if(sensor->pixformat == PIXFORMAT_GRAYSCALE){
            err = make_gray_bmp(req, fb);
        /* To save RAM and CPU in camera ISR use the rgb565 and convert to RGB in the APP */
        }else{
            err = httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
        }
    }

    /* buf_len as 0 to mark that all chunks have been sent. */
    if (err == ESP_OK) {
        err = httpd_resp_send_chunk(req, 0, 0);
    }

    return err;
}

static const httpd_uri_t bmp = {
    .uri       = "/bmp",
    .method    = HTTP_GET,
    .handler   = handle_rgb_bmp,
};


/********** check-qr handler ***********/

static esp_err_t check_qr_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr(req,"Access-Control-Allow-Origin","*");

    char *resp = qr_recognize();

    httpd_resp_send(req, resp, strlen(resp));
    return ESP_OK;
}

static const httpd_uri_t check_qr = {
    .uri       = "/check-qr",
    .method    = HTTP_GET,
    .handler   = check_qr_handler,
    .user_ctx  = NULL
};



static httpd_handle_t start_webserver(void)
{
    rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 111072;
    
    config.uri_match_fn = httpd_uri_match_wildcard;
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &sendsteps);
        httpd_register_uri_handler(server, &setup_sta);
        httpd_register_uri_handler(server, &reset_nvs);
        httpd_register_uri_handler(server, &get_status);
        httpd_register_uri_handler(server, &mdns_name);
        httpd_register_uri_handler(server, &bmp);
        httpd_register_uri_handler(server, &check_qr);

        /* URI handler for getting web server files */
        httpd_uri_t common_get_uri = {
            .uri = "/*",
            .method = HTTP_GET,
            .handler = rest_common_get_handler,
            .user_ctx = rest_context
        };
        httpd_register_uri_handler(server, &common_get_uri);

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