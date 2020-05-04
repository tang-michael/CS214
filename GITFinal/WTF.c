#include "configureHostname.h"
#include "createProject.h"
#include "pushProject.h"
#include "commitProject.h"
#include "addProject.h"
#include "removeProject.h"
#include "manifest.h"
#include "utils.h"
#include "destroyProject.h"
#include "currentversionProject.h"
#include "checkoutProject.h"


int main (int argc, char ** argv)
{
	if (argc < 3 )
	{
		printf("you must put in a valid argument\n");
		exit(0);
	}

 	//==============================FUNCTIONS THAT DO NOT NEED TO CONNECT TO THE SERVER ====================================
	if (strcmp(argv[1],"configure")==0)
	{
		if (argc<4){
			printf("you need to put a valid host name and a port number to connect to\n");
			exit(0);
		}
		configure(argv[2],argv[3]);
		exit(0);
	}
	else if(strcmp(argv[1],"add") == 0)
	{
		if (argc<4){
			printf("you must have a projectname and a filename\n");
			exit(0);
		}
		addFile(argv[2],argv[3]);
		exit(0);
	}
	else if(strcmp(argv[1],"remove")==0)
	{
		if (argc<4){
			printf("you must have a projectname and a filename\n");
			exit(0);
		}
		removeFile(argv[2],argv[3]);
		exit(0);
	}

	readConfigure();
	connect_server(IP, PORT);
	if (strcmp(argv[1],"create")==0){
		clientCreate(argv[2]);
		shutdown(SERVER, SHUT_RDWR);
		close(SERVER);
		exit(0);
	}
	else if(strcmp(argv[1],"commit")==0)
	{
		clientCommit(argv[2]);
		shutdown(SERVER, SHUT_RDWR);
		close(SERVER);
		exit(0);
	}
	else if(strcmp(argv[1],"push")==0)
	{
		clientPush(argv[2]);
		shutdown(SERVER, SHUT_RDWR);
		close(SERVER);
		exit(0);
	}
	else if(strcmp(argv[1],"destroy")==0)
	{
		clientDestroy(argv[2]);
		shutdown(SERVER, SHUT_RDWR);
		close(SERVER);
		exit(0);
	}
	else if(strcmp(argv[1],"currentversion")==0)
	{
		clientCurrentVersion(argv[2]);
		shutdown(SERVER, SHUT_RDWR);
		close(SERVER);
		exit(0);
	}
	else if(strcmp(argv[1],"checkout")==0)
	{
		clientCheckout(argv[2]);
		shutdown(SERVER, SHUT_RDWR);
		close(SERVER);
		exit(0);
	}
	else if(strcmp(argv[1],"rollback")==0)
	{
		clientRollback(argv[2]);
		shutdown(SERVER, SHUT_RDWR);
		close(SERVER);
		exit(0);
	}
	else if(strcmp(argv[1],"update")==0)
	{
		clientUpdate(argv[2]);
		shutdown(SERVER, SHUT_RDWR);
		close(SERVER);
		exit(0);
	}
	else if(strcmp(argv[1],"upgrade")==0)
	{
		clientUpgrade(argv[2]);
		shutdown(SERVER, SHUT_RDWR);
		close(SERVER);
		exit(0);
	}


	return 0;
}
