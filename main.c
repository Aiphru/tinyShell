#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  char buffer[128];
  int lenArgs = 0;
  while (strcmp(buffer, "exit")) {
    char *args[10];

    printf("tinyShell >");
    fgets(buffer, 50, stdin);
    size_t terminatingChar = strcspn(buffer, "\n");
    buffer[terminatingChar] = '\0';

    char *argvArr = strtok(buffer, " ");
    for (int i = 0; argvArr != NULL; i++) {
      args[i] = argvArr;
      printf("%s\n", argvArr);
      argvArr = strtok(NULL, " ");
      lenArgs++;
    }
    args[lenArgs + 1] = NULL;
  }
  return 1;
}
