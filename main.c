#include "main.h"

typedef struct {
  char *name;
  void (*func)(char **args); 
} builtincmds;

extern const builtincmds builtins[];
extern int numBuiltIns;

void builtin_exit(char **args) { exit(0);}

void builtin_help(char **args) {
  printf("Built in commands :\n");
    //}
  for (int i=0;i<numBuiltIns;i++){
    printf("%s\n", builtins[i].name);
  }
}

void builtin_cd(char **args) {
  if (args[1] == NULL){
    char *home = getenv("HOME");
    chdir(home); 
    return;
  }
  chdir(args[1]);//crashes when cd into home(mby a permission issue?)
  return;
}

void builtin_type(char **args){
  if (args[1] == NULL) return;

  for (int i=0;i<numBuiltIns;i++){
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

void printShell(){
  char workingDir[128];
  getcwd(workingDir, sizeof(workingDir));
  printf("tinyShell %s >", workingDir);
}

int parseInput(char *buffer,char **args){
  return 1;
}

void saveHistory(FILE *history, char *buffer){
  history = fopen("History","a");
  fprintf(history, "%s\n", buffer);
  fclose(history);
}

bool checkBuiltIns(char **args,int numBuiltIns){
  for (int i=0; i< numBuiltIns;i++){
    if (strcmp(args[i],builtins[i].name) == 0){
      builtins[i].func(args);
      return true;
    }
  }
    //}
  return false;
}

const builtincmds builtins[] = 
                            {
                            {"help", builtin_help},
                            {"cd", builtin_cd},
                            {"echo", builtin_echo},
                            {"exit", builtin_exit},
                            {"pwd", builtin_pwd},
  			                    {"type",builtin_type}};

int numBuiltIns = sizeof(builtins)/sizeof(builtins[0]);

int main() {
  char buffer[128] = "";
  FILE *history;
  while (1) {
    int lenArgs = 0;
    char *args[20];
    bool isBuiltIn = false;
    
    printShell();
    
    fgets(buffer, 100, stdin);
    size_t terminatingChar = strcspn(buffer, "\n");
    buffer[terminatingChar] = '\0';
    char *argvArr = strtok(buffer, " ");
    for (int i = 0; argvArr != NULL && i<19; i++) {
      args[i] = argvArr;
      argvArr = strtok(NULL, " ");
      lenArgs++;
    }
    if (lenArgs == 0) continue;
    args[lenArgs] = NULL;
    saveHistory(history,buffer);
    isBuiltIn = checkBuiltIns(args,numBuiltIns);
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
