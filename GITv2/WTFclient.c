#include "WTF.h"

int main (int argc, char ** argv){
	if (argc < 3 ){
		printf("you must put in a valid argument\n");
		exit(0);
	}
	node * LL;
	int server;
 	//==============================FUNCTIONS THAT DO NOT NEED TO CONNECT TO THE SERVER ====================================
	if (strcmp(argv[1],"configure")==0){
		if (argc<4){
			printf("you need to put a valid host name and a port number to connect to\n");
			exit(0);
		}
		configure(argv[2],argv[3]);
		exit(0);
	}
	// else if(strcmp(argv[1],"add") == 0){
	// 	if (argc<4){
	// 		printf("you must have a projectname and a filename\n");
	// 		exit(0);
	// 	}
	// 	addFile(argv[2],argv[3]);
	// 	exit(0);
	// }
	// else if(strcmp(argv[1],"remove")==0){
	// 	if (argc<4){
	// 		printf("you must have a projectname and a filename\n");
	// 		exit(0);
	// 	}
	// 	removeFile(argv[2],argv[3]);
	// 	exit(0);
	// }
	// //================================== FUNCTIONS THAT REQUIRE THE SERVER =====================================
	readConfigure();
	connect_server(IP, PORT);
	if (strcmp(argv[1],"create")==0){
		createProject(argv[2]);
		sleep(1);
		shutdown(SERVER, SHUT_RDWR);
		close(SERVER);
		exit(0);
	// }
	// else if (strcmp(argv[1],"push")==0){
	// 	pushProject(argv[2]);
	// 	exit(0);
	// }
	// else if (strcmp(argv[1],"destroy")==0){
	// 	destroyProject(argv[2]);
	// 	exit(0);
	// }
	// else if (strcmp(argv[1],"update")==0){
	// 	updateProject(argv[2]);
	// 	exit(0);
	}
	return 0;
}
