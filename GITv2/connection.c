#include "WTF.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void connect_server (char * Ip, int port){
	int sockfd, portno, n, status, message_size, sent_size, total_size;
    struct sockaddr_in serv_addr;
    struct hostent *server;

	char recv_buffer[1024], send_buffer[1024], *tmp;


	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(Ip);
	if (server == NULL) {
        printf("ERROR, no such host\n");
        exit(0);
    }
	bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    while (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        printf("Error connectin: will retry in 3 seconds \n");

		sleep(3);
	}

	printf("the connection is good\n");

	SERVER = sockfd;
}

/*
* Adds to IPaddress and portno to .configure file
*/
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

void readConfigure(){

	// tokenizes it so that it can get the name and the port
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
