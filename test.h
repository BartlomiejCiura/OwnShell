
void parseCmd(char* cmd, char** params);
int executeCmd(char** params);
int go_cd(char **params);
int go_help(char **params);
int go_history(char **params);
int go_exit(char **params);
int executeBuiltInCommand(char **params); 
int isBuiltInCommand(char** params);
int lsh_num_builtins();
void readCommandLine(char* cmd, size_t cmdSize);

char *builtin_str[] = {
 	"cd",
 	"help",
 	"history",
 	"exit"
};

int (*builtin_func[]) (char **) = {
	&go_cd,
 	&go_help,
	&go_history,
 	&go_exit
};



