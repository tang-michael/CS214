#include "manifest.h"
#include <math.h>

//Creates manifest name with .manifest extension
char* createManifestName(char* projectName)
{
	char* manifestName = malloc((strlen(projectName) + 10) * sizeof(char));
	memset(manifestName, 0x0, (strlen(projectName) + 10));
	strncpy(manifestName, projectName, strlen(projectName));
	strncat(manifestName, ".manifest", 9);
	return manifestName;
}
//uses sha to generate a hash for the project file
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
manEntry *extractEntries(char *text, int trailer)
{
	 int leader = trailer;
	 manEntry *entry = malloc(sizeof(manEntry));
	 while(text[leader] != ','){
		 leader++;
	 }
	 char *version = malloc((leader-trailer)*sizeof(char));
	 strncpy(version, (char*)text+trailer,leader-trailer);
	 entry->version = atoi(version);
	 free(version);
	 leader++;

	 trailer = leader;
	 while(text[leader] != ',')
	 {
		leader++;
	 }
	 entry->name = malloc((leader-trailer)*sizeof(char));
	 memset(entry->name, 0x0, (leader-trailer)*sizeof(char)+1);
	 strncpy(entry->name, (char*)text+trailer,leader-trailer);
	 leader++;

	 trailer = leader;
	 while(text[leader] != '\n'){
 			leader++;
	 }
	 entry->hash = malloc(SHA_DIGEST_LENGTH*2+1);
	 memset(entry->hash, 0x0, (leader-trailer)*sizeof(char)+1);
	 strncpy(entry->hash, (char*)text+trailer,leader-trailer);


	return entry;
}

//读取指定manifest文件中的条目
manEntry **readManifest(char *manifestName, int* size)
{
	struct stat buff;
	stat(manifestName, &buff);

	//将manifest的内容读取到text中
	int contents = open(manifestName, readFlag, 0644);
	char *text = malloc(buff.st_size*sizeof(char)+1);
	memset(text,0x0,buff.st_size*sizeof(char)+1);
	read(contents, text, buff.st_size);
	close(contents);



	int counter = 0;
	int  leader =0, trailer = 0;
	int maxEntries = 100;

	manEntry** entries = malloc(maxEntries*sizeof(manEntry*));

	while(leader < buff.st_size)
	{
		while(text[leader] != '\n')
		{
			leader++;
		}

		entries[counter] = extractEntries(text, trailer);
		counter++;

		leader++;
		//此时的trailer为下一行开始的位置
		trailer = leader;
		if(counter == maxEntries)
		{
			maxEntries = maxEntries + 5;
			entries = realloc(entries, maxEntries*sizeof(manEntry*));
		}
	}

	*size = counter;
	return entries;
}

//Creates a new entry for manifest with filename
manEntry* newManEntry(char* fileName, int version)
{
	//Creates entry
	manEntry* project = malloc(sizeof(manEntry));

	//Set version number
	project->version = version;
	int nLen = floor(log10(project->version)) + 1;

	//Puts name of file into manifest name
	project->name = malloc(strlen(fileName)*sizeof(char) + 1);
	memset(project->name, 0x0, strlen(fileName)*sizeof(char) + 1);
	strncpy(project->name, fileName, strlen(fileName));

	//Generates hash
	struct stat buffer;
	stat(fileName, &buffer);
	if(buffer.st_size == 0)
	{
		project->hash = generateHash(fileName);  //hello/hello.manifest

	}
	else
	{
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

void recursive_newManEntry(int fd, char *pathName, int version)
{
	struct stat s={};
	int res = stat(pathName,&s);
	if(res==-1)
		perror("stat"),exit(-1);
	if(S_ISDIR(s.st_mode))
	{
		DIR *d;
		struct dirent *dir;
		d = opendir(pathName);
		if (d)
		{
			while ((dir = readdir(d)) != NULL)
	        {
				if(strcmp(dir->d_name,".") == 0||strcmp(dir->d_name,"..") == 0 || strcmp(dir->d_name,".git") == 0)
				{
					continue;
				}else
				{
					char path[512]={};
					strcpy(path, pathName);
					strcat(path, "/");
					strcat(path, dir->d_name);
					recursive_newManEntry(fd ,path, version);
				}
	        }
	        closedir(d);
		}
	}
	else if(S_ISREG(s.st_mode))
	{
		manEntry* newProject = newManEntry(pathName, version);
		writeManEntry(newProject, fd);
		freeManEntry(newProject);
	}
}

//Updates manentry for entry
void updateManEntry(manEntry* entry)
{
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
void writeManEntry(manEntry* entry, int fileDescriptor)
{
	int verLen = floor(log10(entry->version))+1;
	char* entryText = malloc((strlen(entry->name)+verLen+40+4)*sizeof(char));
	sprintf(entryText, "%d,%s,%s\n", entry->version, entry->name,entry->hash);
	write(fileDescriptor, entryText, strlen(entryText));
	free(entryText);
}

manEntry* getManEntry(char* rawText, int trailer)
{
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

void compareManifestEntries(manEntry** clientEntry, int clientNumEntries, manEntry** serverEntry, int serverNumEntries)
{
	int version = clientEntry[0]->version - serverEntry[0]->version;
	int i, j;
	char *liveHash;
	if(version == 0)
	{
		int fd = open(".Update", newFlag, 0644);
		remove(".Conflict");
		printf("Up To Date");
		return;
	}
	int exist = 0;
	for(i = 0; i < clientNumEntries; i++)
	{
		exist = 0;
		if(strstr(clientEntry[i]->name, ".manifest") == 0)
		{
			for(j = 0; j < serverNumEntries; j++)
			{
				if(strcmp(clientEntry[i]->name, serverEntry[i]->name) == 0)
				{
					exist = 1;
//					liveHash = genLiveHash(clientEntry[i]->name);
					liveHash = generateHash(clientEntry[i]->name);
					if(version != 0 && serverEntry[j]->version != clientEntry[i]->version && strcmp(clientEntry[i]->hash, serverEntry[j]->hash) != 0)
					{
						clientEntry[i]->code = 'M';
					}
					else if(version != 0 && strcmp(clientEntry[i]->hash,serverEntry[j]->hash) != 0 && strcmp(clientEntry[i]->hash,liveHash) != 0 )
					{
						clientEntry[i]->code = 'C';
					}
				}
				//In client but not server
				if(!exist)
				{
					clientEntry[i]->code = 'D';
				}
			}
		}
	}

	for(j = 0; j < serverNumEntries; j++)
	{
	exist = 0;
	if(strstr(serverEntry[i]->name, ".manifest") == 0)
	{
		for(i = 0; i < clientNumEntries; i++)
		{
			if(strcmp(clientEntry[i]->name, serverEntry[j]->name) == 0)
			{
				exist = 1;
			}
		}
	}
		if(!exist)
		{
			serverEntry[j]->code = 'A';
		}
		else
		{
		  //Serves as a placeholder so theres no errors when writing to update file
		  //Files that already been looked at in previous loop
			serverEntry[j]->code = 'i';
		}
	}
	//Serves as a placeholder so tehres no errors when writing to update file
	clientEntry[0]->code = 'i';
	serverEntry[0]->code  = 'i';
}

void freeManifest(manEntry** manifest, int *size)
{
	int i;
	for(i = 0; i < *size; i++){
		freeManEntry(manifest[i]);
	}
}

void freeManEntry(manEntry* entry)
{
	free(entry->name);
	free(entry->hash);
	free(entry);
}
