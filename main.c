#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>


typedef struct {
  char *name;
  void (*func)(char **args); 
} builtincmds;

extern const builtincmds builtins[];

void builtin_exit(char **args) { exit(0); }

void builtin_cd(char **args) {
  if (args[1] == NULL){
  	chdir("/home/andrea"); // change to user home
  }
  chdir(args[1]);
  return;
}

void builtin_type(char **args){
  if (args[1] == NULL) return;

  for (int i=0;i<5;i++){
    if (strcmp(args[1], builtins[i].name) == 0){
	printf("%s is a builtin command\n",args[1]);
	return;
    }
  }
  char path[64] = "/usr/bin/";
  strcat(path,*(args+1));
  if (access(path, F_OK) == 0){
  	printf("%s is %s\n",args[1],path);
	return;
  }
  printf("type : %s: not found\n",args[1]);
  return;
}

void builtin_pwd(char **args) {
  char pwd[256];
  getcwd(pwd, sizeof(pwd));
  printf("%s\n", pwd);
  return;
}

void builtin_echo(char **args) {
  for (int i = 1; args[i] != NULL; i++) {
    printf("%s ", args[i]);
  }
  printf("\n");
  return;
}

const builtincmds builtins[] = {{"cd", builtin_cd},
                            {"echo", builtin_echo},
                            {"exit", builtin_exit},
                            {"pwd", builtin_pwd},
  			    {"type",builtin_type}};
int main() {
  char buffer[128] = "";
  FILE *history;
  int numBuiltIns = 5;
  while (1) {
    int lenArgs = 0;
    char *args[10];
    char workingDir[128];
    getcwd(workingDir, sizeof(workingDir));
    bool isBuiltIn = false;

    printf("tinyShell %s > ", workingDir);
    fgets(buffer, 100, stdin);
    size_t terminatingChar = strcspn(buffer, "\n");
    buffer[terminatingChar] = '\0';
    char *argvArr = strtok(buffer, " ");
    for (int i = 0; argvArr != NULL; i++) {
      args[i] = argvArr;
      argvArr = strtok(NULL, " ");
      lenArgs++;
    }
    if (lenArgs == 0) continue;
    args[lenArgs] = NULL;
    history = fopen(".tinyShell", "a");
    fprintf(history, "%s\n", buffer);
    fclose(history);

    for (int i = 0; i < numBuiltIns; i++) {
      if (strcmp(args[0], builtins[i].name) == 0) {
        builtins[i].func(args);
        isBuiltIn = true;
        break;
      }
    }

    if (!isBuiltIn) {
      pid_t child_pid = fork();
      if (child_pid == -1) {
        perror("fork");
      }
      if (child_pid == 0) {
        execvp(args[0], args);
      	if (errno == ENOENT){
	  fprintf(stderr,"%s: command not found\n",args[0]);
	}
	else{
	perror("execvp");
      	}
      } 
      else {
        waitpid(child_pid, NULL, 0);
        memset(args, 0, sizeof(args));
      }
    }

    isBuiltIn = false;
  }
  return 0;
}
