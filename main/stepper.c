#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_system.h"
#include "stepper.h"
#include "driver/gpio.h"

int dir[2] = {0,0};
int delayMS = 2;

const bool FULL_STEP_MOTOR_SEQUENCE[][4] = {
		{ 1,  0,  1,  0 },
		{ 0,  1,  1,  0 },
		{ 0,  1,  0,  1 },
		{ 1,  0,  0,  1 },
		{ 1,  0,  1,  0 },
		{ 0,  1,  1,  0 },
		{ 0,  1,  0,  1 },
		{ 1,  0,  0,  1 }
};

const bool HALF_STEP_MOTOR_SEQUENCE[][4] = {
		{ 1,  0,  0,  0 },
		{ 1,  1,  0,  0 },
		{ 0,  1,  0,  0 },
		{ 0,  1,  1,  0 },
		{ 0,  0,  1,  0 },
		{ 0,  0,  1,  1 },
		{ 0,  0,  0,  1 },
		{ 1,  0,  0,  1 }
};


const int PINS1[4] = {GPIO_NUM_16,GPIO_NUM_4,GPIO_NUM_0,GPIO_NUM_2};
const int PINS2[4] = {GPIO_NUM_17,GPIO_NUM_5,GPIO_NUM_18,GPIO_NUM_19};

void setDir(int m1,int m2) {
	dir[0] = m1; //pins1
	dir[1] = m2; //pins2
}


void writeSequence(const unsigned int sequenceNo) {
	for (int i = 0; i < 4; i++) {
		if (dir[0] == -1) gpio_set_level(PINS1[i], FULL_STEP_MOTOR_SEQUENCE[sequenceNo][4-i]);
		else gpio_set_level(PINS1[i], FULL_STEP_MOTOR_SEQUENCE[7-sequenceNo][4-i]);

		if (dir[1] == -1) gpio_set_level(PINS2[i], FULL_STEP_MOTOR_SEQUENCE[sequenceNo][4-i]);
		else gpio_set_level(PINS2[i], FULL_STEP_MOTOR_SEQUENCE[7-sequenceNo][4-i]);
	}
}

void initStepper() {
	for (int i = 0; i < 4; i++) {
    	gpio_set_direction(PINS1[i], GPIO_MODE_OUTPUT);
		gpio_set_direction(PINS2[i], GPIO_MODE_OUTPUT);
    }
}

void makeStep() {
	int sequenceNo = 0;
    for (int i = 0; i < 2048; i++) {
		sequenceNo = (sequenceNo + 1) % 8;
        writeSequence(sequenceNo);
		vTaskDelay(delayMS / portTICK_PERIOD_MS);
    }
}
