#include "WTF.h"

//Creates manifest name with .manifest extension
char* createManifestName(char* projectName){
	char* manifestName = malloc((strlen(projectName)+10)*sizeof(char));
	memset(manifestName, 0x0, (strlen(projectName)+10));
	strncpy(manifestName, projectName, strlen(projectName));
	strncat(manifestName, ".manifest", 9);
	return manifestName;
}
//USes sha to generate a hash for the project file
char* generateHash(char* inputText){
	int i;
	unsigned char temp[SHA_DIGEST_LENGTH];
	char* hash = malloc(SHA_DIGEST_LENGTH*2+1);

	memset(temp, 0x0, SHA_DIGEST_LENGTH);
	memset(hash, 0x0, SHA_DIGEST_LENGTH*2+1);
	SHA1((const char *)inputText, (unsigned long)strlen(inputText), temp);

	for(i = 0; i < SHA_DIGEST_LENGTH; i++){
		sprintf((char*)&(hash[i*2]), "%02x", temp[i]);
	}
	return hash;
}
manEntry* extractEntry(char* rawText, int trailer){
	manEntry* entry = malloc(sizeof(manEntry));

	//Get the version number
	int leader = trailer;
	while(rawText[leader] != ','){
		leader++;
	}
	char* version = malloc((leader-trailer+1)*sizeof(char));
	strncpy(version,(char*)rawText+trailer,(leader-trailer));
	entry->version = atoi(version);
	free(version);
	leader++;

	//Get the file name
	trailer = leader;
	while(rawText[leader] != ','){
		leader++;
	}
	entry->name = malloc((leader-trailer)*sizeof(char)+1);
	memset(entry->name, 0x0, (leader-trailer)*sizeof(char)+1);
	strncpy(entry->name,(char*)rawText+trailer,(leader-trailer));
	leader++;

	//Get the file hash
	trailer = leader;
	while(rawText[leader] != '\n'){
		leader++;
	}
	entry->hash = malloc(SHA_DIGEST_LENGTH*2+1);
	memset(entry->hash, 0x0, (leader-trailer)*sizeof(char));
	strncpy(entry->hash,(char*)rawText+trailer,(leader-trailer));

	return entry;
}
manEntry** readManifest(char* projectName){
	struct stat buffer;
	stat(projectName, &buffer);

	//Read manifest into memory
	int contents = open(projectName, readFlag, mode);
	char* manText = malloc(buffer.st_size*sizeof(char)+1);
	memset(manText, 0x0, buffer.st_size*sizeof(char)+1);
	read(contents, manText, buffer.st_size);
	close(contents);

	// Convert into array of manifest Entries
	int numEntries = 0;
	int maxEntries = 10;
	int leader = 0;
	int trailer = 0;

	manEntry** entryArray = malloc(maxEntries*sizeof(manEntry*));
	while(leader < buffer.st_size && manText[leader] != '\0'){
		while(manText[leader]!='\n' && manText[leader] != '\0'){
			leader++;
		}
		entryArray[numEntries] = extractEntry(manText, trailer);
		numEntries++;

		leader++;
		trailer=leader;

		if(numEntries == maxEntries){
			maxEntries *= 1.2;
			entryArray = realloc(entryArray, maxEntries*sizeof(manEntry));
		}
	}

	MANIFEST_ENTRIES = numEntries;

	return entryArray;
}
//Creates a new entry for mainfest with filename
manEntry* newManEntry(char* fileName){
	//Creates entry
	manEntry* project = malloc(sizeof(manEntry));

	//Set version number
	project->version = 1;
	int nLen = floor(log10(project->version)) + 1;

	//Puts name of file into manifest name
	project->name = malloc(strlen(fileName)*sizeof(char) + 1);
	memset(project->name, 0x0, strlen(fileName)*sizeof(char) + 1);
	strncpy(project->name, fileName, strlen(fileName));

	//Generates hash
	struct stat buffer;
	stat(fileName, &buffer);
	if(buffer.st_size == 0){
		project->hash = generateHash(fileName);
	} else {
		int contents = open(fileName, readFlag, mode);
		char* fileText = malloc(buffer.st_size*sizeof(char) + 1);
		memset(fileText, 0x0, buffer.st_size*sizeof(char) + 1);
		read(contents, fileText, buffer.st_size);
		close(contents);
		project->hash = generateHash(fileText);
		free(fileText);
	}
	//Returns entire man entry for file
	return project;
}

//Updates manentry for entry
void updateManEntry(manEntry* entry){
	//Read file to generate has for it
	struct stat statbuf;
	//Get info about file
	stat(entry->name, &statbuf);
	//Open file into contents to get fd
	int contents = open(entry->name, readFlag);
	char* text = malloc(statbuf.st_size*sizeof(char)+1);
	memset(text, 0x0, statbuf.st_size*sizeof(char)+1);
	//get text from file and store it into string
	read(contents, text, statbuf.st_size);
	close(contents);
	//Generate hash with textfile
	char* temp = generateHash(text);
	free(text);

	//Check if hashes match
	if(strcmp(entry->hash,temp) == 0){
		free(temp);
		exit(0);
	} else {
		//Increase version number and update hash
		entry->version++;
		entry->hash = temp;
		exit(0);
	}
}

//Writes into file the entry
void writeManEntry(manEntry* entry, int fileDescriptor){
	int verLen = floor(log10(entry->version))+1;
	char* entryText = malloc((strlen(entry->name)+verLen+40+4)*sizeof(char));
	sprintf(entryText, "%d,%s,%s\n", entry->version, entry->name,entry->hash);
	write(fileDescriptor, entryText, strlen(entryText));
	free(entryText);
}

manEntry* getManEntry(char* rawText, int trailer){
manEntry* entry = malloc(sizeof(manEntry));

	//Get the version number
	int leader = trailer;
	while(rawText[leader] != ','){
		leader++;
	}
	char* version = malloc((leader-trailer+1)*sizeof(char));
	strncpy(version,(char*)rawText+trailer,(leader-trailer));
	entry->version = atoi(version);
	free(version);
	leader++;

	//Get the file name
	trailer = leader;
	while(rawText[leader] != ','){
		leader++;
	}
	entry->name = malloc((leader-trailer)*sizeof(char)+1);
	memset(entry->name, 0x0, (leader-trailer)*sizeof(char)+1);
	strncpy(entry->name,(char*)rawText+trailer,(leader-trailer));
	leader++;

	//Get the file hash
	trailer = leader;
	while(rawText[leader] != '\n'){
		leader++;
	}
	entry->hash = malloc(SHA_DIGEST_LENGTH*2+1);
	memset(entry->hash, 0x0, (leader-trailer)*sizeof(char));
	strncpy(entry->hash,(char*)rawText+trailer,(leader-trailer));

	return entry;
}


//Free entire manifest
void freeManifest(manEntry** manifest){
	int i;
	for(i = 0; i < MANIFEST_ENTRIES; i++){
		freeManEntry(manifest[i]);
	}
}

//Free one entry of a manifest
void freeManEntry(manEntry* entry){
	free(entry->name);
	free(entry->hash);
	free(entry);
}
