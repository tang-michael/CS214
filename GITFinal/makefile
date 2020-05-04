all: addProject.o checkoutProject.o commitProject.o configureHostname.o createProject.o currentversionProject.o destroyProject.o historyProject.o manifest.o pushProject.o removeProject.o rollbackProject.o updateProject.o upgradeProject.o utils.o WTF.o WTFServer.o
	gcc -g -O -o client/WTF addProject.c checkoutProject.c commitProject.c configureHostname.c createProject.c currentversionProject.c destroyProject.c historyProject.c manifest.c pushProject.c removeProject.c rollbackProject.c updateProject.c upgradeProject.c utils.c WTF.c  -lcrypto -lm -Wunused-result
	gcc -g -O -o server/WTFServer addProject.c checkoutProject.c commitProject.c createProject.c currentversionProject.c destroyProject.c historyProject.c manifest.c pushProject.c removeProject.c rollbackProject.c updateProject.c upgradeProject.c utils.c WTFServer.c -pthread -lm -lcrypto -Wunused-result
#	gcc -g -O -o WTFTest WTFTest.c



clean:
	rm -r client/WTF
	rm -rf client/hello/.git
	rm -r server/WTFServer
	rm -rf server/hello
	rm *.o 





