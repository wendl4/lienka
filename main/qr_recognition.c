#include <stdio.h>
#include <string.h>
#include "quirc_internal.h"
#include "qr_recognition.h"
#include "esp_camera.h"
#include "quirc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

static char* TAG="qr recognize";

void init_camera() {
        static camera_config_t camera_config = {
        .pin_pwdn  = -1,
        .pin_reset = 26,
        .pin_xclk = 13,
        .pin_sscb_sda = 21,
        .pin_sscb_scl = 22,

        .pin_d7 = 35,
        .pin_d6 = 34,
        .pin_d5 = 32,
        .pin_d4 = 12,
        .pin_d3 = 33,
        .pin_d2 = 3,
        .pin_d1 = 25,
        .pin_d0 = 15,
        .pin_vsync = 27,
        .pin_href = 23,
        .pin_pclk = 14,

        .xclk_freq_hz = 12000000,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,
        .pixel_format = PIXFORMAT_GRAYSCALE,
        .frame_size = FRAMESIZE_QQVGA,
        .jpeg_quality = 12,
        .fb_count = 1
    };

    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        return;
    }
}


char* qr_recognize() {
     
    camera_fb_t * fb = esp_camera_fb_get();
    char* output = NULL;
    
    if( fb ){
        printf("begin to qr_recoginze\r\n");
        struct quirc *q;
        struct quirc_data qd;
        uint8_t *image;
        q = quirc_new();

        if (!q) {
	        printf("can't create quirc object\r\n");
        }
        
        printf("begin to quirc_resize\r\n");

        if (quirc_resize(q, fb->width, fb->height) < 0)
        {
	        printf("quirc_resize err\r\n");
        }
        
        image = quirc_begin(q, NULL, NULL);
        memcpy(image, fb->buf, fb->len);
        quirc_end(q);
        printf("quirc_end\r\n");
        
        int id_count = quirc_count(q);
        if (id_count == 0) {
	        fprintf(stderr, "Error: not a valid qrcode\n");
        }

        struct quirc_code code;
        quirc_extract(q, 0, &code);
        quirc_decode_error_t err;
        err = quirc_decode(&code, &qd);
		if (err) {
			printf("  Decoding FAILED: %s\n", quirc_strerror(err));
		} else {
			printf("  Decoding successful:\n");
			printf("payload: %s \n",qd.payload);
            output = strdup((char*) &qd.payload);
		}
        quirc_destroy(q);
        
        esp_camera_fb_return(fb);
    }else {
        ESP_LOGE(TAG, "Camera Capture Failed");    
    }

	printf("finish recoginize\r\n");

    if (!output) {
        output = "Error";
    }
    return output;
}