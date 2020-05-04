#include "configureHostname.h"
#include "utils.h"

void configure ( char* IPAddress, char* port ){
  //First remove .configure file
	int removed = remove( ".configure");
	int port_Number = strtol(port, NULL, 10);
  //Check if port if valid
	if( port_Number<8000 || port_Number>63999){
		printf("Please chose a valid portnumber");
		exit(0);
	}
	int fd;
	fd = open(".configure", O_CREAT | O_RDWR, 0644);
	write(fd,IPAddress, strlen(IPAddress));
	write(fd, " ", 1);
	write(fd, port,strlen(port));
	close(fd);
}
void readConfigure()
{

	int file = open(".configure" ,O_RDONLY, 0644);
	int length = lseek(file,0,SEEK_END);

	lseek(file,0, SEEK_SET);
	char * contents = malloc((length)*sizeof(char));
	int size_of_LL;
	node * ptr= NULL;
	node * head = NULL;
	int status = read(file, contents, length);
	int i,j;
    i=0;

    while(i<strlen(contents)){
        if (isalnum(contents[i])){
            j=i;
            while (!iscntrl(contents[i]) && !isspace(contents[i]) && i<strlen(contents)){
                i++;
            }
            char * new_token= malloc(sizeof(char)* (i-j));
            strncpy(new_token,contents+j,i-j);
			ptr=(node*)malloc(sizeof(node));
			ptr->data=new_token;
			ptr->next= NULL;

			if (head==NULL){
				head = ptr;

			}else {
				head->next = ptr;

			}
		}
		i++;
	}
	close(file);
	IP= head->data;
	PORT = atoi(head->next->data);
}
