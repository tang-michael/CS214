all: client.c connection.c manifest.c server.c WTFclient.c WTFserver.c
	mkdir client
	mkdir server
	gcc -g -O -o client/WTF WTFclient.c  client.c conflict.c update.c upgrade.c connection.c manifest.c currentversion.c -lcrypto -lm
	gcc -g -O -o server/WTFServer WTFserver.c server.c  manifest.c -pthread -lm -lcrypto



clean:
	rm -r client
	rm -r server
	clear
