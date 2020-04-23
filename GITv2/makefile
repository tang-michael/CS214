all: client.c connection.c manifest.c server.c WTFClient.c WTFServer.c
	mkdir client
	mkdir server
	gcc -g -O -o client/WTF WTFClient.c client.c connection.c manifest.c -lcrypto -lm
	gcc -g -O -o server/WTFServer WTFServer.c server.c  manifest.c -pthread -lm -lcrypto
	gcc -g -O -o WTFTest WTFTest.c



clean:
	rm -r client
	rm -r server
	clear
