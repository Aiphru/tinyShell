#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int checkForPipe(char **args)
{
  for (int i = 0; args[i] != NULL; i++)
  {
    if (*args[i] == '|')
    {
      return 1;
    }
  }
  return 0;
}

char ***splitInputIntoPipable(char **args)
{
  char ***splitArgs;
  splitArgs = calloc(2, sizeof(char **));
  splitArgs[0] = calloc(16, sizeof(char *));
  splitArgs[1] = calloc(16, sizeof(char *));
  int foundPipe = 0;
  int arrOneLen = 0;
  int argsIndex = 0;
  for (int i = 0; args[i] != NULL; i++)
  {
    if (*args[i] == '|')
    {
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

int getNumberOfPipes(char **args)
{
  int count = 0;
  for (int i = 0; args[i] != NULL; i++)
  {
    if (*args[i] == '|')
    {
      count++;
    }
  }
  return count;
}

char ***splitInputIntoNPipable(char **args)
{
  char ***splitArgs;
  int numPipes = getNumberOfPipes(args);
  splitArgs = calloc(numPipes + 1, sizeof(char **));
  for (int i = 0; i <= numPipes; i++)
  {
    splitArgs[i] = calloc(16, sizeof(char *));
  }
  int foundPipe = 0;
  int argsIndex = 0;
  for (int i = 0; args[i] != NULL; i++)
  {
    if (*args[i] == '|')
    {
      foundPipe++;
      argsIndex = 0;
      continue;
    }
    splitArgs[foundPipe][argsIndex++] = strdup(args[i]);
  }
  for (int i = 0; i <= numPipes; i++)
  {
    splitArgs[i][argsIndex] = NULL;
  }
  splitArgs[numPipes][0] = NULL;
  return splitArgs;
}

void ***runPipedCommands(char ***pipedArgs, int numPipes)
{
  int pipeFd[numPipes - 1][2];
  pid_t pids[numPipes];
  for (int i = 0; i < numPipes; i++)
  {
    if (i < numPipes - 1)
    {
      if (pipe(pipeFd[i]) == -1)
      {
        perror("pipe");
        exit(EXIT_FAILURE);
      }
    }
    pids[i] = fork();
    if (pids[i] == -1)
    {
      perror("fork");
      exit(EXIT_FAILURE);
    }
    if (pids[i] == 0)
    {
      if (i > 0)
      {
        if (dup2(pipeFd[i - 1][0], STDIN_FILENO) == -1)
        {
          perror("dup2");
          exit(EXIT_FAILURE);
        }
      }
      if (i < numPipes - 1)
      {
        if (dup2(pipeFd[i][1], STDOUT_FILENO) == -1)
        {
          perror("dup2");
          exit(EXIT_FAILURE);
        }
      }
      for (int j = 0; j < numPipes - 1; j++)
      {
        close(pipeFd[j][0]);
        close(pipeFd[j][1]);
      }
      execvp(pipedArgs[i][0], pipedArgs[i]);
      fprintf(stderr, "Error : %s\n", strerror(errno));
      if (errno == ENOENT)
      {
        fprintf(stderr, "%s : command not found\n", pipedArgs[i][0]);
        exit(1);
      }
      for (int i = 0; i < numPipes; i++)
      {
        waitpid(pids[i], NULL, 0);
      }
    }
  }
}
