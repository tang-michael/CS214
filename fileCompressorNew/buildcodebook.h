#ifndef BUILDCODEBOOK_H_
#define BUILDCODEBOOK_H_

typedef struct {

	char* pword;
	int fre;

}item;


void build_codebook(char* );
void build_codebook_recursive(char* );

#endif /* BUILDCODEBOOK_H_ */
