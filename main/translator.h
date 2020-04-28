 typedef struct cmd {
   char name[50];
   int  arg1;
   int  arg2;
} cmd;
 
typedef struct stack {
    int values[512];
    int top;
} stack;

int translate(char * code, cmd * program);