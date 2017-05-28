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
	char cmd2[MAX_COMMAND_LENGTH + 1];
	char* params[MAX_NUMBER_OF_PARAMS + 1];
	int childPid;
	int bg = 0;
	int status = 0;
	char scriptName[100];
	createPathToHistoryFile();
	printf("Type 'help' to check built in commands\n");
	
	while(1) {
		char* username = getenv("USER");
		printf("%s@shell: ", username);

		readCommandLine(cmd, sizeof(cmd));
		saveCommandAsHistory(cmd);
		
		if(cmd[strlen(cmd)-1] == '\n') {
			cmd[strlen(cmd)-1] = '\0';
		}

		if(cmd[strlen(cmd)-1] == '&') {
			bg = 1;
			cmd[strlen(cmd)-1] = '\0';			
		}
	
		if(cmd[0] == '.' && cmd[1] == '/' ){
			for (int i = 0; i < 100; i++){
				scriptName[i] = cmd[i+2];
				if(cmd[i+2] == '\0') break;
			}
			
			char buf[1024];
			FILE *file;
			size_t nread;
			file = fopen("/home/mlody/LinuxShell/scripts/test.sh", "r");
			if (file) {
				int i = 0;
				while(!feof(file)){
					while(fgets(cmd, 512, file)){
						i++;
						if(cmd[strlen(cmd)-1] == '\n') {
							cmd[strlen(cmd)-1] = '\0';
						}
						if (i > 1){
							//////////////////
							parseCmd(cmd, params);
		
							if (isBuiltInCommand(params)) {
								executeBuiltInCommand(params);
							}
							else{
								childPid = fork();
								if (childPid == 0) {
								execvp(params[0], params);  
								printf("%s - Unknown command \n", params[0]);
								return 0;
								}else{
									if (bg == 1) {
										signal(SIGCHLD, SIG_IGN);
									}else{
										wait(NULL);
									}
								}	
							}	
						/////////////////
						}
					}		
				}			
				fclose(file);	
			}
		}else{

			parseCmd(cmd, params);
		
			if (isBuiltInCommand(params)) {
				executeBuiltInCommand(params);
			}
			else{
				childPid = fork();
				if (childPid == 0) {
				execvp(params[0], params);  		
			   printf("%s - Unknown command \n", params[0]);
			   return 0;
				}else{
					if (bg == 1) {
						signal(SIGCHLD, SIG_IGN);
					}else {
						wait(NULL);
					}
				}	
			}
		}
	}
return 0;
}

void readCommandLine(char* cmd, size_t cmdSize) {
	fgets(cmd, cmdSize, stdin);
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
        //waitpid(pid, &childStatus, 0);
        return 1;
    }
}

void saveCommandAsHistory(char* cmd) {
	int ch, number_of_lines = 0;
	FILE *f = fopen(PATH_TO_FILE, "a+");
		
	if (f == NULL)	{
		 printf("Error opening file!\n");
		 exit(1);
	}

	do {
	  ch = fgetc(f);
    	if(ch == '\n') {
       	 number_of_lines++;
        }
	} while (ch != EOF);
	
	if(ch != '\n' && number_of_lines != 0) {
    	number_of_lines++;
	}
	
	if (number_of_lines > 20) {
		
	}
	
	fprintf(f, "%s", cmd);
	fclose(f);
}

void createPathToHistoryFile(void) {
	char dest[50];
	char* uname = getenv("USER");

	strcpy(PATH_TO_FILE, "/home/");
	strcat(PATH_TO_FILE, uname);
	strcpy(dest, "/LinuxShell/history.txt");
	strcat(PATH_TO_FILE, dest);
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

int go_history(char **params) {
	char buf[1024];
	FILE *file;
	size_t nread;


	file = fopen(PATH_TO_FILE, "r");
	if (file) {
		 while ((nread = fread(buf, 1, sizeof buf, file)) > 0)
		     fwrite(buf, 1, nread, stdout);
		 if (ferror(file)) {
		     printf("BLAD");
		 }
		 fclose(file);
	}
}


int go_exit(char **params) {
	exit(EXIT_SUCCESS);
}
