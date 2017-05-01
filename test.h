#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>


void parseCmd(char* cmd, char** params);
int executeCmd(char** params);
int go_cd(char **params);
int go_help(char **params);
int go_exit(char **params);
int executeBuiltInCommand(char **params); 
int isBuiltInCommand(char** params);
int lsh_num_builtins();
void readCommandLine(char* cmd);
