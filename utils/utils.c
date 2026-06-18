#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HOST_NAME_FILE_NAME "/etc/hostname"

char *getName() { return getenv("NAME"); }

char *getHostname(char *buffer, int len) {
  FILE *hostNameFile = fopen(HOST_NAME_FILE_NAME, "r");
  if (hostNameFile == NULL)
    return "";
  fgets(buffer, len, hostNameFile);
  int bufferLen = strcspn(buffer, "\n");
  buffer[bufferLen] = '\0';
  return buffer;
}
