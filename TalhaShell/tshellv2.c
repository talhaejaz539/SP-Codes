#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "TalhaShell:-"
#define BUILTINS 2

int execute(char* arglist[]);
char** tokenize(char* cmdline);
char* read_cmd(char*, FILE*);
const char* builtinCmd[BUILTINS] = {"exit","cd"};

int myexit(int argc, char** argv){
	printf("Exited from TalhaShell\n");
	exit(argc);
}

int mycd(int argc, char** argv){
	int len = strlen(argv[1]);
	char *buff = (char*)malloc(sizeof(char)*len + 8);
	bzero(buff,sizeof(char)* len + 8);
	if(argv[1][0] == '~')
		sprintf(buff,"%s%s",getenv("HOME"),argv[1]+1);
	else 
		sprintf(buff,"%s",argv[1]);
	setenv("PWD",buff,1);
	chdir(buff);
	free(buff);
	return 0;
}

const int (*builtinCmdPtr[BUILTINS])(int, char**) = {myexit, mycd};

int execBuiltin(int i, char **argv){
	int rv =  (*builtinCmdPtr[i])(i,argv);
	return 0;
}

int isBuiltin(const char* cmd){
	for(int i = 0; i<BUILTINS; i++){
		if(!strcmp(builtinCmd[i],cmd)){
			return i;
		}
	}
	return -1;
}

int main() {
	char *cmdline;
	char** arglist;
	char* prompt = PROMPT;
	while((cmdline = read_cmd(prompt, stdin)) != NULL) {
		if((arglist = tokenize(cmdline)) != NULL) {
			execute(arglist);
			for(int j=0; j < MAXARGS+1; j++)
				free(arglist[j]);
			free(arglist);
			free(cmdline);
		}
	}
	printf("\n");
	return 0;
}

int execute(char* arglist[]) {
	int status, rv;
	rv = isBuiltin(arglist[0]);
   	if(rv != -1){
		execBuiltin(rv, arglist);
		if(rv == 0)	return 0;
   	}
	else {
		int cpid = fork();
		switch(cpid) {
		case -1:
		  perror("fork failed");
	       	  exit(1);
		case 0:
		  execvp(arglist[0], arglist);
		  perror("Command not found....");
		  exit(1);
		default:
	          waitpid(cpid, &status, 0);
		  //printf("child exited with status %d \n", status >> 8);
		  return 0;
	}	
	}
}
char** tokenize(char* cmdline) {
	char** arglist = (char**)malloc(sizeof(char*)*(MAXARGS+1));
	for(int j=0; j < MAXARGS+1; j++) {
		arglist[j] = (char*)malloc(sizeof(char)*ARGLEN);
		bzero(arglist[j], ARGLEN);
	}
	if(cmdline[0] == '\0')		return NULL;
	int argnum = 0;
	char* cp = cmdline;
	char* start;
	int len;
	while(*cp != '\0') {
		while(*cp == ' ' || *cp == '\t')
			cp++;
		start = cp;
		len = 1;
		while(*++cp != '\0' && !(*cp == ' ' || *cp == '\t'))
			len++;
		strncpy(arglist[argnum], start, len);
		arglist[argnum][len] == '\0';
		argnum++;
	}
	arglist[argnum] = NULL;
	return arglist;
}
char* read_cmd(char* prompt, FILE* fp) {
	printf("%s", prompt);
	int c;
	int pos = 0;
	char* cmdline = (char*)malloc(sizeof(char)*MAX_LEN);
	while((c = getc(fp)) != EOF) {
		if(c == '\n')
			break;
		cmdline[pos++] = c;
	}
	if(c == EOF && pos == 0)
		return NULL;
	cmdline[pos] = '\0';
	return cmdline;
}
