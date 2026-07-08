#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HOST_NAME_FILE_NAME "/etc/hostname"

char *getName() { return getenv("NAME"); }

char *getHostname(char *buffer, int len)
{
  FILE *hostNameFile = fopen(HOST_NAME_FILE_NAME, "r");
  if (hostNameFile == NULL)
    return "";
  fgets(buffer, len, hostNameFile);
  int bufferLen = strcspn(buffer, "\n");
  buffer[bufferLen] = '\0';
  return buffer;
}

char *trimWhiteSpace(char *string)
{
  int len = strlen(string);
  while (len > 0 && string[len - 1] == ' ')
  {
    string[len - 1] = '\0';
    len--;
  }
  return string;
}

void freePipedArgs(char ***pipedArgs, int numPipes)
{
  for (int i = 0; i < numPipes; i++)
  {
    for (int j = 0; pipedArgs[i][j] != NULL; j++)
    {
      free(pipedArgs[i][j]);
    }
    free(pipedArgs[i]);
  }
  free(pipedArgs);
}

void freeArguments(char **arguments)
{
  for (int i = 0; arguments[i] != NULL; i++)
  {
    free(arguments[i]);
  }
  free(arguments);
}

void executeCommand(char **arguments)
{
  signal(SIGINT, SIG_IGN);
  pid_t child_pid = fork();
  if (child_pid == -1)
  {
    perror("fork");
    exit(1);
  }
  if (child_pid == 0)
  {
    signal(SIGINT, SIG_DFL);
    execvp(arguments[0], arguments);
    if (errno == ENOENT)
    {
      fprintf(stderr, "%s: command not found\n", arguments[0]);
    }
    else
    {
      perror("execvp");
      _exit(child_pid);
    }
  }
  else
  {
    waitpid(child_pid, NULL, 0);
    signal(SIGINT, SIG_DFL);
  }
}
