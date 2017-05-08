#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "test.h"

#define MAX_COMMAND_LENGTH 100
#define MAX_NUMBER_OF_PARAMS 10


int main()
{
	char cmd[MAX_COMMAND_LENGTH + 1];
	char* params[MAX_NUMBER_OF_PARAMS + 1];
	int childPid;
	int bg = 0;
	int status = 0;
	printf("Type 'help' to check built in commands\n");

	while(1) {
		char* username = getenv("USER");
		printf("%s@shell: ", username);

		readCommandLine(cmd, sizeof(cmd));
		if(cmd[strlen(cmd)-1] == '&') {
			bg = 1;
			cmd[strlen(cmd)-1] = '\0';			
		}

	
		parseCmd(cmd, params);
		


		if (isBuiltInCommand(params)) {
			executeBuiltInCommand(params);
		}
		else {
			childPid = fork();
			if (childPid == 0) {

		//		if(bg == 1) {
			//		fclose(stdin);
			//		fopen("/dev/null", "r");
		//			execvp(params[0], params);  
	//			} else {
			//	executeCmd(params)
				execvp(params[0], params);  
			//	printf("\n");
				
	        printf("%s - Unknown command \n", params[0]);
	        return 0;
	     //   }
			} else {
					if (bg == 1) {
						signal(SIGCHLD, SIG_IGN);
					} else {
					//	waitpid(childPid, &status, 0);
						wait(NULL);
					}
					//	waitpid (childPid);
					}	
			}
		
	}
return 0;
}


void readCommandLine(char* cmd, size_t cmdSize) {
	fgets(cmd, cmdSize, stdin);

	char* uname = getenv("USER");
	char filee1[] = "/home/";
	char filee2[] = "/LinuxShell/history.txt";
	char* allfile = strncat(filee1, uname, sizeof(char[100]));
	char* allfile2 = strncat(allfile, filee2, sizeof(char[100]));
	
	FILE *f = fopen(allfile2, "a");
	if (f == NULL)
	{
		 printf("Error opening file!\n");
		 exit(1);
	}

	fprintf(f, "%s", cmd);
	fclose(f);

	if(cmd[strlen(cmd)-1] == '\n') {
		cmd[strlen(cmd)-1] = '\0';
	}
}


void parseCmd(char* cmd, char** params) {       
    for(int i = 0; i < MAX_NUMBER_OF_PARAMS; i++) {
        params[i] = strsep(&cmd, " ");
        if(params[i] == NULL) break;
    }
}


int isBuiltInCommand(char** params) {
	int i;
	for (i = 0; i < lsh_num_builtins(); i++) {
   	if (strcmp(params[0], builtin_str[i]) == 0) {
   	   return 1;
    	}
  	}	
	return 0;
}


int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}


int executeBuiltInCommand(char **params) {
	int i;
	for (i = 0; i < lsh_num_builtins(); i++) {
   	if (strcmp(params[0], builtin_str[i]) == 0) {
   	   return (*builtin_func[i])(params);
    	}
  	}	
	return 1;
}


int executeCmd(char** params) {
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
   //     char* error = strerror(errno);
        printf("%s - Unknown command \n", params[0]);
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


int go_help(char **params) {
	int i;
	printf("The following commands are built in:\n");

	for (i = 0; i < lsh_num_builtins(); i++) {
		printf("--> %s\n", builtin_str[i]);
  	}
  	return 1;
}


int go_exit(char **params) {
	exit(EXIT_SUCCESS);
}




