#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void save_token(char **args, int *argCount, char *buf, int *bufCount){
  buf[*bufCount] = '\0';
  args[*argCount] = strdup(buf);
  (*argCount)++;
  *bufCount = 0;
}

char** parseInput(char *command){
  char *commandCpy = strdup(command);
  commandCpy[strcspn(commandCpy, "\n")] = '\0';
  char buffer[1024] = {0};
  char quoteChar = 0;
  char *arguments[64];
  int argumentCount = 0;
  int bufferCount = 0;
  bool inQuotes = false;
  for (int i=0;i<strlen(commandCpy);i++){
    if ((commandCpy[i] == '"' || commandCpy[i] == '\'') && !inQuotes){
      quoteChar = commandCpy[i];
      inQuotes = true;
      continue;
    }
    if (inQuotes && commandCpy[i] == quoteChar) {
      save_token(arguments, &argumentCount, buffer, &bufferCount);
      inQuotes = false;
      continue;
    }
    if (!inQuotes && commandCpy[i] == ' '){
      if (bufferCount == 0) continue;
      save_token(arguments, &argumentCount, buffer, &bufferCount);
      continue;
    }
    buffer[bufferCount++] = commandCpy[i];
  }
  buffer[bufferCount]= '\0';
  arguments[argumentCount] = strdup(buffer);
  char **result = malloc(sizeof(char*) * (argumentCount+2));
  for (int i=0;i<=argumentCount;i++){
    result[i] = arguments[i];
  }
  free(commandCpy);
  for (int i=0;i<=argumentCount;i++){
  printf("[%d] : %s  ", i, result[i]);
  }
  return result;
}

#ifndef TEST_UNIT
int main(){
  char buffer[MAX_BUFFER_SIZE] = "";
  fgets(buffer, MAX_BUFFER_SIZE, stdin);
  char **ARGS_ARRAY = parseInput(buffer);
}
#endif
