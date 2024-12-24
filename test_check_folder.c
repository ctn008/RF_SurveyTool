/**
 * C program to check whether a directory exists or not.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int isDirectoryExists(const char *path);


int main()
{
    char path[100];
	char *filename = "test.uint8";
	FILE *file;

    printf("Enter directory path: ");
    scanf("%s", path);


    // Check if directory exists or not
    if (isDirectoryExists(path))
    {
        printf("Directory exists at path '%s'\n", path);
    }
    else
    {
        printf("Creating new directory path '%s'\n", path);
        // Creating a directory
    	if (mkdir(path, 0777) == -1)
        	printf("Error! Not able to createnew directory path '%s'\n", path); 
	    else
    	    printf("Directory created\n");
    }
	char slash[] = "/";
	printf("path length: %li\n", strlen(path));
	if (path[strlen(path)-1] != slash[0]){
		strcat(path, slash);
		printf("run till here\n");
	}
	strcat(path, filename);
	printf("path & filename: %s", path);
	
	file = fopen(path, "wb");
	if (!file) {
		fprintf(stderr, "Failed to open %s\n", filename);
	}
	else {
		fclose(file);
	}
    return 0;
}



/**
 * Function to check whether a directory exists or not.
 * It returns 1 if given path is directory and  exists 
 * otherwise returns 0.
 */
int isDirectoryExists(const char *path)
{
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
}
