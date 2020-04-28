#include "translator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <esp_log.h>

int CHAR_0 = 48;
int CHAR_9 = 57;
int CHAR_A = 97;
int CHAR_Z = 122;
int TOKEN_MAX_LENGTH = 6;

static const char *TAG = "translate";

char * scan(char * prog, char * token) {
    while (*prog != '\0') {
        if (*prog >= CHAR_0 && *prog <= CHAR_9) {
            while (*prog >= CHAR_0 && *prog <= CHAR_9) {
                token[strlen(token)] = *prog;
                prog++;
            }
            return token;
        }
        else if (*prog >= CHAR_A && *prog <= CHAR_Z) {
            token[strlen(token)] = *prog;
           
            if (strcmp(token,"count") == 0 ||
                strcmp(token,"begin") == 0 ||
                strcmp(token,"end") == 0 ||
                strcmp(token,"up") == 0 ||
                strcmp(token,"down") == 0 ||
                strcmp(token,"left") == 0 ||
                strcmp(token,"right") == 0) {
                return token;
            }
        }
        prog++;    
    }
    return "none";
}
 
char * next(char * code,char * token) {
    memset(token,0,TOKEN_MAX_LENGTH);
    scan(code,token);
    return (code + strlen(token));
}
 
int translate(char code[], cmd * program) {
    char token[128] = "";
    
    stack jump_stack;
    jump_stack.top = 0;
    ESP_LOGI(TAG,"code - %s",code);
    int curr_register = 0;
    int pc = 0;
    
    while (*code != '\0') {
        code = next(code,token);

        cmd curr_cmd;
        curr_cmd.arg1 = -1;
        curr_cmd.arg2 = -1;

        if (strcmp(token,"count") == 0) {
            strcpy(curr_cmd.name,"MOV");
           
            code = next(code,token);
           
            curr_cmd.arg1 = curr_register;
            curr_cmd.arg2 = atoi(token);
            memcpy(&program[pc], &curr_cmd, sizeof(curr_cmd));
            pc++;
        }
        
        else if (strcmp(token,"begin") == 0) {
            strcpy(curr_cmd.name, "JZ");
            curr_cmd.arg1 = curr_register++;
            curr_cmd.arg2 = -1;
            memcpy(&program[pc], &curr_cmd, sizeof(curr_cmd));
           
            jump_stack.values[jump_stack.top] = pc;
            jump_stack.top++;
            pc++;
        }
       
        else if (strcmp(token,"end") == 0) {
            strcpy(curr_cmd.name,"DECR");
            curr_register--;
            curr_cmd.arg1 = curr_register;
            curr_cmd.arg2 = -1;
            memcpy(&program[pc], &curr_cmd, sizeof(curr_cmd));
            pc++;
           
            strcpy(curr_cmd.name,"JMP");
            jump_stack.top--;
            curr_cmd.arg1 = jump_stack.values[jump_stack.top];
            program[jump_stack.values[jump_stack.top]].arg2 = pc+1;
            memcpy(&program[pc], &curr_cmd, sizeof(curr_cmd));
            pc++;
        }
       
        else if (strcmp(token,"up") == 0) {
            strcpy(curr_cmd.name,"FWD");
            memcpy(&program[pc], &curr_cmd, sizeof(curr_cmd));
            pc++;
        }
       
        else if (strcmp(token,"down") == 0) {
            strcpy(curr_cmd.name,"BCK");
            memcpy(&program[pc], &curr_cmd, sizeof(curr_cmd));
            pc++;
        }
       
        else if (strcmp(token,"left") == 0) {
            strcpy(curr_cmd.name,"LFT");
            memcpy(&program[pc], &curr_cmd, sizeof(curr_cmd));
            pc++;
        }
       
        else if (strcmp(token,"right") == 0) {
            strcpy(curr_cmd.name,"RGHT");
            memcpy(&program[pc], &curr_cmd, sizeof(curr_cmd));
            pc++;
        }
        if (*code == '\0') {
            strcpy(curr_cmd.name,"HLT");
            curr_cmd.arg1 = -1;
            curr_cmd.arg2 = -1;
            memcpy(&program[pc], &curr_cmd, sizeof(curr_cmd));
        }
    }
    return pc;
}