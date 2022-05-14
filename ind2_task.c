#define failure(str) {perror(str); exit(-1);}
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int numProc = 0, numRun = 0;

//copying file from fork
void fileCopy(char* srcName, char* destName) {
	if (numRun >= numProc) {
		if (wait(NULL) == -1) {
			perror("Can not wait for process");
			numRun--;
			numProc--;
			return;
		}
		
		numRun--;
	}
	numRun++;
	
	pid_t child;
	switch(child = fork()){
		case -1:
			perror("Child process could not be created");
			break;
		case 0:
			if (execlp("./filecpy.run", "filecpy.run", srcName, destName, NULL) == -1)
				failure("File copying failure");
	}
}

//create full file name
char* createFileName(char* fullPath, char* fileName) {
	char* Buffer;
	if ((Buffer = (char*)malloc((strlen(fullPath)+strlen(fileName)+1)*sizeof(char))) == NULL) {
		perror("Can not allocate a memory");
		return "";
	}
	strcpy(Buffer, fullPath);
	
	if (Buffer[strlen(Buffer)-1]!='/') 
		strcat(Buffer, "/");
		
	strcat(Buffer, fileName);
	return Buffer;
}

int main(int argc, char *argv[]) {
	if (argc < 4)
		failure("Invalid arguments\nFormat: ind2.run [srcDir] [destDir] [number of processes]");
		

	if (!sscanf(argv[3], "%u", &numProc) || (numProc < 1))
		failure("Invalid number of processes");
		
	DIR *srcDir, *destDir;
	
	//open directories
	if ((srcDir = opendir(argv[1])) == NULL)
		failure("Can not open srcDir");
		
	if ((destDir = opendir(argv[2])) == NULL) {
		closedir(srcDir);
		failure("Can not open destDir");
	}
		
	char srcFull[PATH_MAX];
	char destFull[PATH_MAX];
	
	strcpy(srcFull, argv[1]);
	strcpy(destFull, argv[2]);
	
	struct dirent *srcDirent, *destDirent;
	
	while ((srcDirent = readdir(srcDir)) != NULL) {
		if (srcDirent->d_type == DT_REG) {		
			int equal = 0;
			while ((destDirent = readdir(destDir)) != NULL) 
				if ((strcmp(srcDirent->d_name, destDirent->d_name) == 0) && (destDirent->d_type == DT_REG)) {
					equal = 1;
					break;
				}
			if (!equal) {
				char* srcFileName = createFileName(srcFull, srcDirent->d_name);
				char* destFileName = createFileName(destFull, srcDirent->d_name);
				fileCopy(srcFileName, destFileName);
				free(srcFileName);
				free(destFileName);
			}
			rewinddir(destDir);
		}
	}
				
	//waiting for all processes
	for (int i = numRun; i > 0; i--)
		if (wait(NULL) == -1)
			perror("Can not wait a process");
	
	//close directories
	if (closedir(srcDir))
		failure("Can not close srcDir");
		
	if (closedir(destDir))
		failure("Can not close destDir");
		
	return 0;
}
