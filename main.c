#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  char buffer[128];
  int lenArgs = 0;
  while (strcmp(buffer, "exit")) {
    char *args[10];
    printf("tinyShell >");

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

    pid_t child_pid = fork();
    if (child_pid == -1) {
      perror("fork");
    }
    if (child_pid == 0) {
      execvp(args[0], args);
      perror("execvp");
      printf("command has taken control of this child process. If this is "
             "printed execvp encountered an error.");
    } else {
      waitpid(child_pid, NULL, 0);
      printf("\n");
    }
  }
  return 0;
}
