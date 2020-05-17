#include "perform.h"
#include "translator.h"
#include "stepper.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "esp_log.h"

static const char *TAG = "Perform";

void run(cmd * program) {
    int registers[16]; 
    int position = 0;
    while (strcmp(program[position].name,"HLT") != 0) {
        if (strcmp(program[position].name,"FWD") == 0) {
            setDir(1,1);
            makeMove();
            position++;
        }
        else if (strcmp(program[position].name,"BCK") == 0) {
            setDir(-1,-1);
            makeMove();
            position++;
        }
        else if (strcmp(program[position].name,"LFT") == 0) {
            setDir(-1,1);
            makeMove();
            position++;
        }
        else if (strcmp(program[position].name,"RGHT") == 0) {
            setDir(1,-1);
            makeMove();
            position++;
        }
        else if (strcmp(program[position].name,"MOV") == 0) {
            registers[program[position].arg1] = program[position].arg2;
            position++;
        }
        else if (strcmp(program[position].name,"JMP") == 0) {
            position = program[position].arg1;
        }
        else if (strcmp(program[position].name,"JZ") == 0) {
            if (registers[program[position].arg1] == 0) {
                position = program[position].arg2;
            }
            else position++;
        }
        else if (strcmp(program[position].name,"DECR") == 0) {
            registers[program[position].arg1]--;
            position++;
        }
    }
}

void show(cmd * program, int length) {
    for (int i=0; i<length; i++) {
        ESP_LOGI(TAG,"%s %d %d", program[i].name,program[i].arg1,program[i].arg2);
    }
}

void prepair_program(char code[]) {
    cmd program[128];
    translate(code,program);
    run(program);
}
