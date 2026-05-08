#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
  char *name;
  void (*func)(char **args);
} builtincmds;

void builtin_cd(char **args) { return; }
void builtin_echo(char **args) { return; }
int main() {
  builtincmds builtins[] = {{"cd", builtin_cd}, {"echo", builtin_echo}};
  char buffer[128] = "";
  while (strcmp(buffer, "exit")) {
    int lenArgs = 0;
    char *args[10];
    printf("tinyShell > ");
    fgets(buffer, 100, stdin);
    size_t terminatingChar = strcspn(buffer, "\n");
    buffer[terminatingChar] = '\0';

    char *argvArr = strtok(buffer, " ");
    for (int i = 0; argvArr != NULL; i++) {
      args[i] = argvArr;
      argvArr = strtok(NULL, " ");
      lenArgs++;
    }
    args[lenArgs] = NULL;
    if (strcmp(args[0], "exit\n")) {
      return 0;
    }
    pid_t child_pid = fork();
    if (child_pid == -1) {
      perror("fork");
    }
    if (child_pid == 0) {
      execvp(args[0], args);
      perror("execvp");
    } else {
      waitpid(child_pid, NULL, 0);
      memset(args, 0, sizeof(args));
    }
  }
  return 0;
}
