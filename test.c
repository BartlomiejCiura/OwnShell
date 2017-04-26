#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


void parseCmd(char* cmd, char** params);
int executeCmd(char** params);
int go_cd(char** params) {
	if (params[1] == NULL) {
		fprintf(stderr, "ERROR: expected argument to \"cd\"\n");
	} else {
		if (chdir(params[1]) != 0) {
			perror("ERROR");
		}
	}
	return 1;
}

#define MAX_COMMAND_LENGTH 100
#define MAX_NUMBER_OF_PARAMS 10

int main()
{
    char cmd[MAX_COMMAND_LENGTH + 1];
    char* params[MAX_NUMBER_OF_PARAMS + 1];

    while(1) {
        // Print command prompt
        char* username = getenv("USER");
        printf("%s@shell: ", username);

        // Read command from standard input, exit on Ctrl+D
        if(fgets(cmd, sizeof(cmd), stdin) == NULL) break;

        // Remove trailing newline character, if any
        if(cmd[strlen(cmd)-1] == '\n') {
            cmd[strlen(cmd)-1] = '\0';
        }

        // Split cmd into array of parameters
        parseCmd(cmd, params);
        // Exit?
        if(strcmp(params[0], "exit") == 0) break;

        // Execute command
			if(strcmp(params[0], "cd") == 0) {
				go_cd(params);
			}
         else if(executeCmd(params) == 0) break;
    }

    return 0;
}

// Split cmd into array of parameters
void parseCmd(char* cmd, char** params)
{       
    for(int i = 0; i < MAX_NUMBER_OF_PARAMS; i++) {
        params[i] = strsep(&cmd, " ");
        if(params[i] == NULL) break;
    }
}

int executeCmd(char** params)
{
    // Fork process
    pid_t pid = fork();

    // Error
    if (pid == -1) {
        char* error = strerror(errno);
        printf("fork: %s\n", error);
        return 1;
    }

    // Child process
    else if (pid == 0) {
        // Execute command
	     execvp(params[0], params);  

        // Error occurred
        char* error = strerror(errno);
        printf("bledzik: %s: %s\n", params[0], error);
        return 0;
    }

    // Parent process
    else {
        // Wait for child process to finish
        int childStatus;
        waitpid(pid, &childStatus, 0);
        return 1;
    }
}
