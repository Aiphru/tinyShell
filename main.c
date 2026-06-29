#include "headers.h"

#define SHELL_NAME "tinyShell"
#define HISTORY_FILE "/readLinesHistory"
#define MAX_BUFFER 1024
#define MAX_ARGS 64

typedef struct {
  char *name;
  int (*func)(char **args);
} builtincmds;

extern const builtincmds builtIns[];
extern int numBuiltIns;

int builtinExit(char **args) { exit(0); }

int builtinHelp(char **args) {
  printf("Built in commands :\n");
  for (int i = 0; i < numBuiltIns; i++) {
    printf("%s\n", builtIns[i].name);
  }
  return 0;
}

int builtinCd(char **args) {
  const char *home;

  if (args[1] == NULL) {
    home = getenv("HOME");
    if (home == NULL) {
      return 0;
    }
    chdir(home);
    return 0;
  }
  int res = chdir(args[1]);
  if (res != 0) {
    fprintf(stderr, "No such file or directory\n");
    return -1;
  }
  return 0;
}

int builtinType(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "Usage: %s <command>\n", args[0]);
    return -1;
  }
  for (int i = 0; i < numBuiltIns; i++) {
    if (strcmp(args[1], builtIns[i].name) == 0) {
      printf("%s is a builtin command\n", args[1]);
      return 0;
    }
  }
  char *path[16];
  int count = 0;
  char *pathStr = getenv("PATH");
  if (pathStr == NULL)
    return -1;
  char *copyPath = strdup(pathStr);
  char *item = strtok(copyPath, ":");
  while (item != NULL) {
    path[count++] = item;
    item = strtok(NULL, ":");
  }

  for (int i = 0; i < count; i++) {
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path[i], args[1]);
    if (access(fullpath, F_OK) == 0) {
      printf("%s is %s\n", args[1], fullpath);
      free(copyPath);
      return 0;
    }
  }
  printf("type : %s: not found\n", args[1]);
  free(copyPath);
  return -1;
}

int builtinPwd(char **args) {
  char pwd[256];
  getcwd(pwd, sizeof(pwd));
  printf("%s\n", pwd);
  return 0;
}

int builtinEcho(char **args) {
  for (int i = 1; args[i] != NULL; i++) {
    printf("%s ", args[i]);
  }
  printf("\n");
  return 0;
}

void printShell() {
  char *name = getenv("LOGNAME");
  char hostName[64];
  getHostname(hostName, 64);
  char workingDir[MAX_BUFFER];
  getcwd(workingDir, sizeof(workingDir));
  printf("%s@%s %s >", name, hostName, workingDir);
}

int saveHistory(char *buffer) {
  char *dest = getenv("HOME");
  char destCpy[1024];
  strcpy(destCpy, dest);
  strcat(destCpy, HISTORY_FILE);
  FILE *history = fopen(destCpy, "a");
  if (history == NULL) {
    return 0;
  }
  if (buffer[0] == '\n') {
    fclose(history);
    return -1;
  }
  fprintf(history, "%s", buffer);
  fclose(history);
  return 1;
}

bool checkBuiltIns(char **args, int numBuiltIns) {
  for (int i = 0; i < numBuiltIns; i++) {
    if (strcmp(args[0], builtIns[i].name) == 0) {
      builtIns[i].func(args);
      return true;
    }
  }
  return false;
}

const builtincmds builtIns[] = {{"help", builtinHelp}, {"cd", builtinCd},
                                {"echo", builtinEcho}, {"exit", builtinExit},
                                {"pwd", builtinPwd},   {"type", builtinType}};

int numBuiltIns = sizeof(builtIns) / sizeof(builtIns[0]);

int main() {
  char buffer[MAX_BUFFER] = "";
  while (1) {
    bool isBuiltIn = false;

    printShell();

    fgets(buffer, MAX_BUFFER, stdin);
    saveHistory(buffer);
    char **arguments = parseInput(buffer);
    if (arguments == NULL) {
      free(arguments);
      continue;
    }
    if (checkForPipe(arguments)) {
      char ***pipedArguments = splitInputIntoPipable(arguments);
      continue;
    }
    isBuiltIn = checkBuiltIns(arguments, numBuiltIns);
    if (!isBuiltIn) {
      signal(SIGINT, SIG_IGN);
      pid_t child_pid = fork();
      if (child_pid == -1) {
        perror("fork");
        exit(1);
      }
      if (child_pid == 0) {
        signal(SIGINT, SIG_DFL);
        execvp(arguments[0], arguments);
        if (errno == ENOENT) {
          fprintf(stderr, "%s: command not found\n", arguments[0]);
        } else {
          perror("execvp");
          _exit(child_pid);
        }
      } else {
        waitpid(child_pid, NULL, 0);
        signal(SIGINT, SIG_DFL);
      }
    }
    isBuiltIn = false;
    for (int i = 0; arguments[i] != NULL; i++) {
      free(arguments[i]);
    }
    free(arguments);
  }
  return 0;
}
