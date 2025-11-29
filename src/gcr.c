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


int strArrLen(char* arr[]) {
  if (arr == NULL) {
    return 0;
  }

  int chk = 0;
  while (arr[chk] != NULL) {
    chk++;
  }

  return chk;
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
    help();
    return err("\nnot enough args");
  }

  bool run = false;
  char *file = NULL;
  int argCap = 4;
  char **args = (char **)malloc(sizeof(char *) * argCap);
  if (args == NULL) {
    return err("args malloc failed");
  }

  int argCount = 0;
  args[argCount++] = strdup("tmp");
  if (args == NULL) {
    free(args);
    return err("strdup failed on tmp arg");
  }

  int takCap = 4;
  int *taken = (int *)malloc(sizeof(int) * takCap);
  if (taken == NULL) {
    free(args[0]);
    free(args);
    free(taken);
    return err("taken args failed on malloc");
  }
  int takCount = 0;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "run") == 0) {
      if (i +1 < argc && argv[i+1] != NULL) {
        if (!run && !intArrContains(taken, takCount, i+1)) {
          run = true;
        }
        if (takCount == takCap) {
          takCap *= 2;
          int *newTak = (int *)realloc(taken, sizeof(int) * takCap);
          if (newTak == NULL) {
            return err("realloc taken args failed");
          }
          taken = newTak;
        }
        taken[takCount++] = i;
        file = argv[i+1];

        if (takCount == takCap) {
          takCap *= 2;
          int *newTak = (int *)realloc(taken, sizeof(int) * takCap);
          if (newTak == NULL) {
            return err("realloc taken args failed");
          }
          taken[takCount++] = i+1;
        } else if (!run && !intArrContains(taken, takCount, i)) {
          run = true;
          if (takCount == takCap) {
            takCap *= 2;
            int *newTak = (int *)realloc(taken, sizeof(int) * takCap);
            if (newTak == NULL) {
              return err("realloc taken args failed");
            }
            taken = newTak;
          }
          taken[takCount++] = i;
        }
      } else if (!intArrContains(taken, takCount, i) && !run) {
        if (argCount == argCap) {
          argCap *= 2;
          char **newArg = (char **)realloc(args, sizeof(char *) * argCap);
          if (newArg == NULL) {
            for (int j = 0; j < argCount; j++) {
              free(args[j]);
            }
            free(args);
            free(taken);
            return err("failed to realloc args");
          }
          args = newArg;
        }
        args[argCount++] = argv[i];
        
        if (takCount == takCap) {
          takCap *= 2;
          int *newTak = (int *)realloc(taken, sizeof(int) * takCap);
          if (newTak == NULL) {
            return err("realloc taken args failed");
          }
          taken = newTak;
        }
        taken[takCount++] = i;
      }
    }
    for (int i = 0; i < argCount; i++) {
      printf("%s\n", args[i]);
      return 0;
    }/**/for (int i = 0; i < argCount; i++) { free(args[i]); };free(args);free(taken);/**/
    /*
      if (run == false && !intArrContains(taken, M, i+1)) {
        run = true;
        taken[M] = i;
      };if (argv[i+1] != NULL) {
        taken[M] = i;
        file = argv[i+1];
      }
    } else if (!intArrContains(taken, M, i) && run == false) {
      int lenArgs = strArrLen(args);
      taken[M] = i;
      args[lenArgs+1] = argv[i];
    }*/
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
  } else if (run) {
    return err("not enough args; need file");
  } else {
    if (sizeof(args) == 0) {
      return err("not enough args");
    }

    int totalArgs = strArrLen(args)-1;

    int avgSize = 0;
    for (int i = 0; i < totalArgs; i++) {
      printf("foo\n");
      avgSize += sizeof(args[i]);
    };avgSize = avgSize / totalArgs;

    int argsSize = /*sizeof(args) /*/ avgSize;

    int totalCmdSize = strlen("gcc");
    for (int i = 0; i < totalArgs; i++) {
      printf("%d\n", i);
      totalCmdSize += strlen(args[i]);
      printf("%d\n", i);
    }

    char cmd[totalCmdSize];
    printf("%d\n", totalCmdSize);
    sprintf(cmd, "gcc");
    for (int i = 0; i < argsSize; i++) {
      printf("%d:  \"%s\"  ;  \"\"\n", i, cmd); 
      char tmp[sizeof(" ")+1] = " ";
      printf("%d:  \"%s\"  ;  \"%s\"\n", i, cmd, tmp); 
      sprintf(tmp, " %s", args[i]);
      printf("%d:  \"%s\"  ;  \"%s\"\n", i, cmd, tmp);/* 
      strcat(cmd, tmp);
      printf("%d:  %s  ;  %s\n", i, cmd, tmp); */
    } 

    return system(cmd);
  }

  return 0;
}
