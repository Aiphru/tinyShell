#include "main.h"
#include <stdlib.h>

typedef struct {
  char *name;
  int (*func)(char **args);
} builtincmds;

extern const builtincmds builtins[];
extern int numBuiltIns;

#define SHELL_NAME "tinyShell"
#define MAX_BUFFER 128
#define MAX_ARGS 25

int builtin_exit(char **args) { 
  exit(0);
  return 0;
}

int builtin_help(char **args) {
  printf("Built in commands :\n");
  for (int i = 0; i < numBuiltIns; i++) {
    printf("%s\n", builtins[i].name);
  }
  return 0;
}

int builtin_cd(char **args) {
  const char *home;

  if (args[1] == NULL) {
    home = getenv("HOME");
    if (home == NULL) {
      return 0;
    }
    chdir(home);
    return 0;
  }
  int res = chdir(args[1]);
  if (res != 0){
    fprintf(stderr,"No such file or directory\n");
    return -1;
  }
  return 0;
}

int builtin_type(char **args) {
  if (args[1] == NULL){
    fprintf(stderr, "Usage: %s <command>\n",args[0]);
    return -1; 
  }

  for (int i = 0; i < numBuiltIns; i++) {
    if (strcmp(args[1], builtins[i].name) == 0) {
      printf("%s is a builtin command\n", args[1]);
      return 0;
    }
  }
  char path[128] = "/usr/bin/";
  if (strlen(args[1])>(sizeof(path)-10)){
    fprintf(stderr, "Provided args is too long\n");  
    return -1;
  }
  strcat(path, *(args + 1));
  if (access(path, F_OK) == 0) {
    printf("%s is %s\n", args[1], path);
    return 0;
  }
  printf("type : %s: not found\n", args[1]);
  return 1;
}

int builtin_pwd(char **args) {
  char pwd[256];
  getcwd(pwd, sizeof(pwd));
  if (pwd == NULL){
    return 1;
  }
  printf("%s\n", pwd);
  return 0;
}

int builtin_echo(char **args) {
  for (int i = 1; args[i] != NULL; i++) {
    printf("%s ", args[i]);
  }
  printf("\n");
  return 0;
}

void printShell() {
  char workingDir[MAX_BUFFER];
  getcwd(workingDir, sizeof(workingDir));
  printf("%s %s >", SHELL_NAME, workingDir);
}

// int parseInput(char *buffer, char **args) { return 1; }

int saveHistory(char *buffer) {
  char *dest = getenv("HOME");
  char destCpy[128];
  strcpy(destCpy, dest);
  strcat(destCpy, "/History");
  FILE *history = fopen(destCpy, "a");
  if (history == NULL) {
    return 0;
  }
  fprintf(history, "%s\n", buffer);
  fclose(history);
  return 1;
}

bool checkBuiltIns(char **args, int numBuiltIns) {
  for (int i = 0; i < numBuiltIns; i++) {
    if (strcmp(args[0], builtins[i].name) == 0) {
      builtins[i].func(args);
      return true;
    }
  }
  return false;
}

const builtincmds builtins[] = {{"help", builtin_help}, {"cd", builtin_cd},
                                {"echo", builtin_echo}, {"exit", builtin_exit},
                                {"pwd", builtin_pwd},   {"type", builtin_type}};

int numBuiltIns = sizeof(builtins) / sizeof(builtins[0]);

int main() {
  char buffer[MAX_BUFFER] = "";
  while (1) {
    int lenArgs = 0;
    char *args[MAX_ARGS];
    bool isBuiltIn = false;

    printShell();

    fgets(buffer, MAX_BUFFER, stdin);
    size_t terminatingChar = strcspn(buffer, "\n");
    buffer[terminatingChar] = '\0';
    char *argvArr = strtok(buffer, " ");
    for (int i = 0; argvArr != NULL && i < 19; i++) {
      args[i] = argvArr;
      argvArr = strtok(NULL, " ");
      lenArgs++;
    }
    if (lenArgs == 0)
      continue;
    args[lenArgs] = NULL;
    saveHistory(buffer);
    isBuiltIn = checkBuiltIns(args, numBuiltIns);
    if (!isBuiltIn) {
      pid_t child_pid = fork();
      if (child_pid == -1) {
        perror("fork");
        exit(1);
      }
      if (child_pid == 0) {
        execvp(args[0], args);
        if (errno == ENOENT) {
          fprintf(stderr, "%s: command not found\n", args[0]);
        } else {
          perror("execvp");
        }
      } else {
        waitpid(child_pid, NULL, 0);
        memset(args, 0, sizeof(args));
      }
    }

    isBuiltIn = false;
  }
  return 0;
}
