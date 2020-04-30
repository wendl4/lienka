#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_system.h"
#include "stepper.h"
#include "driver/gpio.h"

const bool FULL_STEP_MOTOR_SEQUENCE[][4] = {
		{ 0,  0,  0,  1 },
		{ 0,  0,  1,  0 },
		{ 0,  1,  0,  0 },
		{ 1,  0,  0,  0 },
};

const bool HALF_STEP_MOTOR_SEQUENCE[][4] = {
		{ 0,  0,  0,  1 },
		{ 0,  0,  1,  1 },
		{ 0,  0,  1,  0 },
		{ 0,  1,  1,  0 },
		{ 0,  1,  0,  0 },
		{ 1,  1,  0,  0 },
		{ 1,  0,  0,  0 },
		{ 1,  0,  0,  1 }
};

// DIRECTION OF MOTORS
int dir[2] = {0,0};
// SEQUENCE LENGTH
const int SEQ_LEN = sizeof(FULL_STEP_MOTOR_SEQUENCE) / sizeof(FULL_STEP_MOTOR_SEQUENCE[0]);
// NUMBER OF PINS FOR EACH MOTOR
const int NUMBER_OF_PINS = sizeof(FULL_STEP_MOTOR_SEQUENCE[0]) / sizeof(FULL_STEP_MOTOR_SEQUENCE[0][0]);
// DELAY AFTER gpio_set_level
const int DELAY_MS = 2;
// PI value
const double M_PI = 3.14159265359;
// WHEEL RADIUS IN mm
const int RADIUS = 25;
// ONE STEP LENGTH IN mm
const int STEP_LENGTH = 150;
// DISTANCE BETWEEN WHEELS in mm
const int WHEEL_DIST = 70;

// ONE ROTATION OF WHEEL CONSISTS OF 2048 PHASES
const int ROTS_PER_MM = (int)2048/(2*M_PI*RADIUS);
const int ROTS_PER_STEP = ROTS_PER_MM * STEP_LENGTH;
const int ROTS_PER_TURN = ROTS_PER_MM * ((2*M_PI*WHEEL_DIST)/8);

const int PINS1[4] = {GPIO_NUM_16,GPIO_NUM_4,GPIO_NUM_0,GPIO_NUM_2};
const int PINS2[4] = {GPIO_NUM_17,GPIO_NUM_5,GPIO_NUM_18,GPIO_NUM_19};

void setDir(int m1,int m2) {
	dir[0] = m1; //pins1
	dir[1] = m2; //pins2
}

void writeSequence(const unsigned int sequenceNo) {
	for (int i = 0; i < NUMBER_OF_PINS; i++) {
		if (dir[0] == -1) {
			gpio_set_level(PINS1[i], FULL_STEP_MOTOR_SEQUENCE[sequenceNo][i]);
		}
		else if (dir[0] == 1) {
			gpio_set_level(PINS1[i], FULL_STEP_MOTOR_SEQUENCE[SEQ_LEN-sequenceNo-1][i]);
		}

		if (dir[1] == -1) {
			gpio_set_level(PINS2[i], FULL_STEP_MOTOR_SEQUENCE[SEQ_LEN-sequenceNo-1][i]);
		}
		else if (dir[1] == 1) {
			gpio_set_level(PINS2[i], FULL_STEP_MOTOR_SEQUENCE[sequenceNo][i]);
		}
	}
}

void initStepper() {
	for (int i = 0; i < NUMBER_OF_PINS; i++) {
    	gpio_set_direction(PINS1[i], GPIO_MODE_OUTPUT);
		gpio_set_direction(PINS2[i], GPIO_MODE_OUTPUT);
    }
}

void endSequence() {
	for (int pinNum = 0; pinNum < NUMBER_OF_PINS; pinNum++) {
		gpio_set_level(PINS1[pinNum],0);
		gpio_set_level(PINS2[pinNum],0);
	}
}

void makeMove() {
	int sequenceNo = 0;
	int rotations = (dir[0]*dir[1] > 0) ? ROTS_PER_STEP : ROTS_PER_TURN;
    for (int i = 0; i < rotations; i++) {
		sequenceNo = (sequenceNo + 1) % SEQ_LEN;
        writeSequence(sequenceNo);
		vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
    }
	endSequence();
}
