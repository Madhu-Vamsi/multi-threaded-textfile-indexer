//headers
#include<pthread.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>


void listFiles(const char *path);

int main()
{
    // Directory path to list files
    char path[100];

    // Input path from user
    printf("Enter path to list files: ");
    scanf("%s", path);

    listFiles(path);

    return 0;
}


/**
 * Lists all files and sub-directories at given path.
 */
void listFiles(const char *base_path)
{
	char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(base_path);
    // Unable to open directory stream
    if (!dir) 
        return; 

    int ret;
    char last_ext[999];
    char dummy_file_name[999];
    char dummy_base_path[999];
    while ((dp = readdir(dir)) != NULL)
    {
    	if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
    	{
	    	dummy_file_name[0]=0;
	    	dummy_base_path[0]=0;
	    	last_ext[0]=0;
	    	char* file_name=dp->d_name;
	    	strcpy(path, base_path);
            strcat(path, "/");
            strcat(path, dp->d_name);
            listFiles(path);
	    	strcpy(dummy_file_name,file_name);
			char* p1=strtok(dummy_file_name,".");
			while(p1!=NULL){
				strcpy(last_ext,p1);
				p1=strtok(NULL,".");

			}
			ret = strcmp(last_ext,"txt");
			if(ret==0)
			{
			    strcpy(dummy_base_path,base_path);
				strcat(dummy_base_path,"/");
				printf("%s\n",strcat(dummy_base_path,file_name));
			}
    	}	
    }
    // Close directory stream
    closedir(dir);
}
