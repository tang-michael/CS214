#include <pthread.h>
#include <signal.h>
#include "utils.h"
#include "manifest.h"
#include "createProject.h"
#include "destroyProject.h"
#include "commitProject.h"
#include "checkoutProject.h"
#include "pushProject.h"

#define	LISTENINGNUMBER	10

void process_signal(int sockfd)
{
	close(sockfd);
//	sleep(1);
	printf("server down\n");
	exit(0);
}
void check_arg(char* command, int fd){

    if(strcmp(command,"destroy") == 0)
    {
        serverDestroy(fd);
    }
    else if(strcmp(command,"create") == 0)
    {
        serverCreate(fd);
    }
    else if(strcmp(command,"commit") == 0)
    {
    	serverCommit(fd);
    }
		else if(strcmp(command,"update") == 0)
    {
    	serverupdate(fd);
    }

    else if(strcmp(command,"push") == 0)
    {
		serverPush(fd);
    }
    else if(strcmp(command,"checkout") == 0)
    {
		serverCheckout(fd);
    }
    else if(strcmp(command,"currentversion") == 0)
    {
    	serverCurrentVersion(fd);
    }
		else if(strcmp(command,"history") == 0)
    {
    	serverHistory(fd);
    }


}

void* func(void *vargp)
{
    int* fd = (int*)vargp;
    int buffer;
    char* command = getFrom(*fd);
    check_arg(command,*fd);
    free(command);
    close(*fd);
    return NULL;
}

int main(int argc, char *argv[])
{
	if (argc<2){
		printf("you must put in a port number\n");
		return 0;
	}
	if (atoi(argv[1])<8000 || atoi(argv[1])>64000){
		printf("PORT must be between 8k adn 64k\n");
		return 0;
	}

	printf("press 'ctrl+c' to exit!!!!\n");
	signal(SIGINT,process_signal);

    int sockfd, newsockfd, portno;
    socklen_t clilen_addr_size;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t thread;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    int a = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &a , sizeof(int)) < 0){

    }
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    else
    	printf("bind ok\n");
    listen(sockfd, LISTENINGNUMBER);
    clilen_addr_size = sizeof(cli_addr);
    while (1)
    {
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen_addr_size);
        if (newsockfd < 0)
            error("ERROR on accept");
        pthread_create(&thread, NULL, func, (void*)(&newsockfd));

    }
    return 0;
}
