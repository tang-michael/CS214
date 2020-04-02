#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// #include "compressor.h"
// #include "frequency.h"
// #include "huffman.h"
// #include "mineheap.h"
// #include "token.h"
#include "DLL.h"


//Compares flag to see
int find_flag(int argc, char*argv[], char *flag ){
  int i = 0;
  for(i = 0; i < argc; i++){
    if(strcmp(flag, argv[i]) == 0){
      return i;
    }
  }
  return -1;
}
void set_flag(int argc, char *argv[], char *flag, int *condition){
  int flag_value = find_flag(argc, argv, flag);
  *condition = flag_value != -1;
}
int main(int argc, char *argv[]){
  int BUILD_CODEBOOK = 0, COMPRESS = 0, DECOMPRESS = 0, RECURSIVE = 0;
  char *INPUT_FILE = NULL;
  char *CODEBOOK_FILE = NULL;

  set_flag(argc, argv, "-b", &BUILD_CODEBOOK);
  set_flag(argc, argv, "-c", &COMPRESS);
  set_flag(argc, argv, "-d", &DECOMPRESS);
  set_flag(argc, argv, "-R", &RECURSIVE);

  // printf("%i\n", BUILD_CODEBOOK);
  // printf("%i\n", COMPRESS);
  // printf("%i\n", DECOMPRESS);
  // printf("%i\n", RECURSIVE);

  if(!(BUILD_CODEBOOK || COMPRESS || DECOMPRESS)){
    printf("Invalid arguments");
  }
  if((BUILD_CODEBOOK && COMPRESS) || (BUILD_CODEBOOK &&  DECOMPRESS) || (COMPRESS && DECOMPRESS)){
    printf("Invalid combination of arguments");
  }

  if (COMPRESS || DECOMPRESS) {
      INPUT_FILE = argv[argc - 2];
      CODEBOOK_FILE = argv[argc - 1];
  } else {
      INPUT_FILE = argv[argc - 1];
  }
  DIR *d;
  struct dirent *dir;
  d = opendir(INPUT_FILE);
  if (d)
  {
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
  }

  // printf("%s\n", INPUT_FILE);
  // printf("%s\n", CODEBOOK_FILE);

}
