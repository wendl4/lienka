#include <stdbool.h>

enum CmdType {
    None = 0,
    StepperFwd,
    StepperBack,
    StepperRight,
    StepperLeft,
    Increment,
    Decrement,
    JumpIfZero,
    Jump,
    Assign
};

void prepair_program(char program[]);
