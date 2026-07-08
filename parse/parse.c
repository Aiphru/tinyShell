#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

char *trimWhiteSpace(char *string);

void save_token(char **args, int *argCount, char *buf, int *bufCount) {
  buf[*bufCount] = '\0';
  args[*argCount] = strdup(buf);
  (*argCount)++;
  *bufCount = 0;
}

char **parseInput(char *command) {
  char *commandCpy = strdup(command);
  char buffer[1024] = {0};
  char quoteChar = 0;
  char *arguments[64];
  int argumentCount = 0;
  int bufferCount = 0;
  bool inQuotes = false;
  commandCpy[strcspn(commandCpy, "\n")] = '\0';
  int commandCpyLength = strlen(commandCpy);
  for (int i = 0; i < commandCpyLength; i++) {
    if ((commandCpy[i] == '"' || commandCpy[i] == '\'') && !inQuotes) {
      quoteChar = commandCpy[i];
      inQuotes = true;
      continue;
    }
    if (inQuotes && commandCpy[i] == quoteChar) {
      save_token(arguments, &argumentCount, buffer, &bufferCount);
      inQuotes = false;
      continue;
    }
    if (!inQuotes && commandCpy[i] == ' ') {
      if (bufferCount == 0)
        continue;
      save_token(arguments, &argumentCount, buffer, &bufferCount);
      continue;
    }
    buffer[bufferCount++] = (commandCpy[i]);
  }
  buffer[bufferCount] = '\0';
  if (bufferCount > 0) {
    arguments[argumentCount] = strdup(buffer);
    argumentCount++;
  }
  if (argumentCount == 0) {
    free(commandCpy);
    return NULL;
  }
  char **result = malloc(sizeof(char *) * (argumentCount + 1));
  for (int i = 0; i < argumentCount; i++) {
    result[i] = arguments[i];
  }
  result[argumentCount] = NULL;
  free(commandCpy);
  return result;
}
