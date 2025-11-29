#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

int err(char* str) {
  char strFin[256];
  sprintf(strFin, "%s\n", str);
  fprintf(stderr, strFin);
  return 1;
}

bool intArrContains(int arr[], int size, int target) {
  if (size == 0) {
    return false;
  }

  for (int i = 0; i < size; i++) {
    if (arr[i] == target) {
      return true;
    }
  }

  return false;
}

void help(void) {
  char *lines[] = {
    "help:",
    "just uses \033[34mgcc\033[0m under the hood",
    "standard \033[34mgcc\033[0m args available",
    "\033[0;34mrun\033[0m is the only addition (similar to \033[34mgo run\033[0m)"
  };
  int N = sizeof(lines) / sizeof(lines[0]);
  for (int i = 0; i < N; i++) {
    printf("%s\n", lines[i]);
  }
}

char* ranStr(int len) {
  char* possible = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0987654321";
  if (len <= 0) {
    return NULL;
  }

  int possLen = strlen(possible);
  char* res = (char*)malloc(sizeof(char) * (len+1));

  if (res == NULL) {
    return NULL;
  }

  for (int i = 0; i < len; i++) {
    int index = rand() % possLen;
    res[i] = possible[index];
  }; res[len] = '\0'; //NULL terminate
  
  return res;
}

char* mkTmp(void) {
  int success = 0;
  int len = sizeof("foobarbazquxquuz");
  char *tmp;

  do {
    tmp = ranStr(16);
    success = access(tmp, F_OK);
  } while (success == 0);

  return tmp;
}

int del(char* file) {
  return remove(file);
}

int ex(char* file) {
  char cmd[256];
  int cmdSize = sizeof("./") + sizeof(file);
  if (cmdSize > 256) {
    return err("buffer overflow prevented");
  }
  sprintf(cmd, "./%s", file);
  return system(cmd);
}

int comp(char* file, char *tmp) {
  char cmd[256];
  int cmdSize = sizeof("gcc ") + sizeof(file) + sizeof(" -o ") + sizeof(tmp);
  if (cmdSize > 256) {
    return err("buffer overflow prevented");
  }
  sprintf(cmd, "gcc %s -o %s", file, tmp);
  return system(cmd);
}


int main(int argc, char *argv[]) {
  srand(time(NULL));
  if (argc < 2) {
    return err("not enough args");
  }

  bool run = false;
  char *file = NULL;
  char *args[] = {"gcc"};
  int taken[] = {1};
  for (int i = 0; i < argc; i++) {
    int M = sizeof(taken) / sizeof(taken[0]);
    if (strcmp(argv[i], "run") == 0) {
      if (!run && !intArrContains(taken, M, i+1)) {
        run = true;
        int lenTak = sizeof(taken) / sizeof(taken[0]);
        taken[lenTak] = i;
      };if (argv[i+1] != NULL) {
        int lenTak = sizeof(taken) / sizeof(taken[0]);
        taken[lenTak] = i;
        file = argv[i+1];
      }
    } else if (!intArrContains(taken, M, i+1)) {
      int lenArgs = sizeof(args) / sizeof(args[0]);
      if (lenArgs == 1 && args[0] == "gcc") {
        lenArgs = 0;
      }
      args[lenArgs] = argv[i];
    }
  }

  if (run && file != NULL) {
    char *tmp = mkTmp();

    if (comp(file, tmp) == 1) {
      return err("err compiling file");
    }
    
    if (ex(tmp) == 1) {
      return err("err executing file");
    }

    if (del(tmp) == 1) {
      return err("deleting tmp file");
    }
  } else {
    int argsSize = sizeof(args) / sizeof(args[0]);

    int totalCmdSize = 0;
    for (int i = 0; i < argsSize; i++) {
      totalCmdSize += sizeof(" ") + sizeof(args[i]);
    }; totalCmdSize += sizeof('\0');

    char cmd[totalCmdSize];
    sprintf(cmd, "gcc");
    for (int i = 0; i < argsSize; i++) {
      sprintf(cmd, "%s %s", cmd, args[i]);
    }

    return system(cmd);
  } 

  return 0;
}
