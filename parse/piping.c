#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int checkForPipe(char **args) {
  for (int i = 0; args[i] != NULL; i++) {
    if (*args[i] == '|') {
      return 1;
    }
  }
  return 0;
}

char ***splitInputIntoPipable(char **args) {
  char ***splitArgs;
  splitArgs = malloc(2 * sizeof(char **));
  splitArgs[0] = malloc(16 * sizeof(char *));
  splitArgs[1] = malloc(16 * sizeof(char *));
  int foundPipe = 0;
  int arrOneLen = 0;
  int argsIndex = 0;
  for (int i = 0; args[i] != NULL; i++) {
    if (*args[i] == '|') {
      arrOneLen = argsIndex + 1;
      foundPipe = 1;
      argsIndex = 0;
      continue;
    }
    splitArgs[foundPipe][argsIndex++] = strdup(args[i]);
  }
  splitArgs[0][arrOneLen] = NULL;
  splitArgs[foundPipe][argsIndex] = NULL;
  return splitArgs;
}
