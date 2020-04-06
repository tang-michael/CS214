#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buildcodebook.h"
#include "compress.h"
#include "decompress.h"

int find_flag(int argc, char*argv[], char *flag )
{
	int i = 0;
	for(i = 0; i < argc; i++)
	{
		if(strcmp(flag, argv[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}
void set_flag(int argc, char *argv[], char *flag, int *condition)
{
	int flag_value = find_flag(argc, argv, flag);
	*condition = flag_value != -1;
}
int main(int argc, char *argv[])
{
	int BUILD_CODEBOOK = 0, COMPRESS = 0, DECOMPRESS = 0, RECURSIVE = 0;
	int sum_flag = 0;

	set_flag(argc, argv, "-b", &BUILD_CODEBOOK);
	set_flag(argc, argv, "-c", &COMPRESS);
	set_flag(argc, argv, "-d", &DECOMPRESS);
	set_flag(argc, argv, "-R", &RECURSIVE);

	sum_flag = BUILD_CODEBOOK + COMPRESS*2 + DECOMPRESS*4 + RECURSIVE*8 + argc*16;

	switch(sum_flag)
	{
	case 1+3*16: 	//-b  3

		build_codebook(argv[argc - 1]);
		break;
	case 9 + 4*16:		//-b -R   4

		build_codebook_recursive(argv[argc - 1]);
		break;
	case 2 + 4*16:		//-c  4
		compress(argv[argc - 2],argv[argc - 1]);
		break;
	case 10 + 5*16:	//-c -R  5
		compress_recursive(argv[argc - 2],argv[argc - 1]);
		break;
	case 4 + 4*16:		//-d 4
		decompress(argv[argc - 2],argv[argc - 1]);
		break;
	case 12 + 5*16:	//-d -R  5
		decompress_recursive(argv[argc - 2],argv[argc - 1]);
		break;
	default:
		printf("Invalid arguments\n");
		break;
	}


	return 0;

}
