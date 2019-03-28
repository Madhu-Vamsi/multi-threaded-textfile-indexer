//author: Madhu Vamsi
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <semaphore.h>


#define MAX_UNIQUE_WORDS (10000)
#define MAX_WORD_LENGTH (300)
#define MAX_FILE_PATH_LENGTH (500)
#define MAX (100) //buffer lenght


int N;
long tf;
sem_t empty;
sem_t full;
sem_t mutex;
char tmp[MAX_FILE_PATH_LENGTH];
char buffer[MAX][MAX_FILE_PATH_LENGTH];
int fill=0;
int use=0;
char mpath[MAX_FILE_PATH_LENGTH];


void put(char* i){
	strcpy(buffer[fill],i);
	fill=(fill+1)%MAX;
}

char* get(){
	strcpy(tmp,buffer[use]);
	use=(use+1)%MAX;
	return tmp;
}

//basic node structure
typedef struct __node_t {
 char word[MAX_WORD_LENGTH];
 int count;
 struct __node_t *next;
} node_t;

typedef struct __word_list_t {
	node_t* head;
	int size;
	pthread_mutex_t lock;
} word_list_t;

word_list_t* main_list;
word_list_t* top_ten_list;

void init_word_list(word_list_t* L){
	L->head=NULL;
	L->size=0;
	pthread_mutex_init(&L->lock, NULL);
}


int lookup_word_list(word_list_t* L, char* str){
	int rv=-1;
	pthread_mutex_lock(&L->lock);
	node_t *curr = L->head;
	while (curr) {
		if(strcmp(curr->word,str)==0){
			rv=1;
			break;
		}
		curr=curr->next;
	}
	pthread_mutex_unlock(&L->lock);
	return rv;
}

void insert_word_in_word_list(word_list_t* L, char* str){
	pthread_mutex_lock(&L->lock);
	int rv=-1;
	node_t* curr=L->head;
	while(curr){
		if(strcmp(curr->word,str)==0){
			curr->count=curr->count + 1;
			rv = 1;
			break;
		}
		curr=curr->next;
	}
	if(L->size>=MAX_UNIQUE_WORDS){
		printf("too many words\n" );
		return;
	}
	if(rv == -1){
		node_t* new_node=(node_t*)malloc(sizeof(node_t));
		if(new_node == NULL){
			printf("malloc FAILED \n" );
			return ;
		}
		strcpy(new_node->word,str);
		new_node->count = 1;
		L->size = L->size + 1;
		new_node->next = L->head;
		L->head = new_node;
	}
	pthread_mutex_unlock(&L->lock);
}

void insert_word_in_word_list_with_count(word_list_t* L, char* str, int val){
	pthread_mutex_lock(&L->lock);
	int rv=-1;
	node_t* curr=L->head;
	while(curr){
		if(strcmp(curr->word,str)==0){
			curr->count=curr->count + val;
			rv = 1;
			break;
		}
		curr=curr->next;
	}
	if(L->size>=MAX_UNIQUE_WORDS){
		printf("too many words\n" );
		return;
	}
	if(rv == -1){
		node_t* new_node=(node_t*)malloc(sizeof(node_t));
		if(new_node == NULL){
			printf("malloc FAILED \n" );
			return ;
		}
		strcpy(new_node->word,str);
		new_node->count = val;
		L->size = L->size + 1;
		new_node->next = L->head;
		L->head = new_node;
	}
	pthread_mutex_unlock(&L->lock);
}

void insert_word_list_in_word_list(word_list_t* main_list, word_list_t* small_list){
	if(small_list==NULL)return;
	if(main_list==NULL){
		main_list=small_list;
		return;
	}
	node_t* curr = small_list->head;
	while(curr){
		insert_word_in_word_list_with_count(main_list,curr->word, curr->count);
		curr = curr->next;
	}
	return;
}


void print_word_list(word_list_t* L){
	if(L==NULL){
		printf("list empty!\n");
		return;
	}
	node_t* curr = L->head;
	while(curr){
	printf("%s\t%d\n", curr->word, curr->count);
	curr = curr-> next;		
	}
}



int is_alpha_numeric(char* c){
	int rv = 0;
	if(*c >='a' && *c<='z'|| *c >= 'A' && *c <= 'Z')
		{
			rv = 1;
		}
	else if(*c =='0'||*c =='1'||*c =='2'||*c =='3'||*c =='4'||*c =='5'||*c =='6'||*c =='7'||*c =='8'||*c =='9')
		{
			rv = 1;
		}
	return rv;
}

void process_word(char* word){
	int i=0;
	for(;i<strlen(word);i++){
		word[i]=tolower(word[i]);
	}
}


word_list_t* file_to_word_list(char* file_path){
	FILE *f;
	char c;
	word_list_t* small_list=(word_list_t*)malloc(sizeof(word_list_t));
	init_word_list(small_list);
	f=fopen(file_path,"r");
	char curr_word[MAX_WORD_LENGTH];
	while(((c=getc(f))!=EOF))
	{
		int test;
		test = is_alpha_numeric(&c);
		if(test == 1){
			int len=strlen(curr_word);
			curr_word[len]=c;
			curr_word[len+1]='\0';
		}
		else {
			process_word(curr_word);
			insert_word_in_word_list(small_list, curr_word);
			curr_word[0]='\0';
		}
	}
	fclose(f);
	return small_list;
}



long cfil(const char *base_path)
{
	char path[1000]; 
	long rv = 0;
    struct dirent *dp;
    DIR *dir = opendir(base_path);
    // Unable to open directory stream
    if (!dir) 
        return 0; 

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
            rv+=cfil(path);
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
				rv++;
			}
    	}	
    }
    // Close directory stream
    closedir(dir);
    return rv;
}



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
                strcat(dummy_base_path,file_name);
                sem_wait(&empty);
                sem_wait(&mutex);
                put(dummy_base_path);
                sem_post(&mutex);
                sem_post(&full);
            }
        }   
    }
    // Close directory stream
    closedir(dir);
}

void print_top_ten(word_list_t* L){
	int count=0; int greater_than_ten_flag=0;
	int max;
	int i=0;
	pthread_mutex_lock(&L->lock);
	node_t *curr = L->head;
	top_ten_list=(word_list_t*)malloc(sizeof(word_list_t));
	init_word_list(top_ten_list);
	int j=0;
	for(;j<10;j++){
		max=0;
		curr=L->head;
		while(curr){
			if(curr->count>max){
				max=curr->count;
			}
			curr=curr->next;
		}
		curr=L->head;
		while(curr){
			if(curr->count==max){
				insert_word_in_word_list_with_count(top_ten_list,curr->word,curr->count);
				curr->count=0;
				break;
			}
			curr=curr->next;
		}	
	}
	print_word_list(top_ten_list);
	pthread_mutex_unlock(&L->lock);
}

void* producer(void* arg){
	listFiles(mpath);
}

void* consumer(void* arg){
	int i;
	for(i=0;i<tf/N;i++){
		sem_wait(&full);
		sem_wait(&mutex);
		char* tmp=get();
		word_list_t* tc = file_to_word_list(tmp);
		insert_word_list_in_word_list(main_list , tc);
		sem_post(&mutex);
		sem_post(&empty);
	}
}

void terminating(){
		printf("usage: <program-name> <directory/file path>\n");
		printf("main is terminating!\n");
		exit(0);
}

int main(int argc, char** argv){
	if(argc<2||argc>4||argc==3){
		printf("bad arguments\n");
		terminating();
	}

	int rc;
	N=2;

	if(argc==2){
		strcpy(mpath,argv[1]);
	}
	else if(argc == 4){
		if(strcmp(argv[1],"-t")!=0)
			{	
				printf("invalid switch\n" );
				terminating();
			}
		strcpy(mpath,argv[3]);
		N=atoi(argv[2]);
	}
	sem_init(&mutex, 0, 1);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, MAX);
	pthread_t p;
	pthread_t consumers[N];
    tf = cfil(mpath);
    main_list=(word_list_t*)malloc(sizeof(word_list_t));
	init_word_list(main_list);
	rc = pthread_create(&p, NULL, producer, NULL);
	assert(rc==0);
	int k=1;
	for(;k<=N;k++){
		rc = pthread_create(&consumers[k], NULL, consumer, NULL);
		assert(rc==0);	
	}
	pthread_join(p,NULL);
	k=1;
	for(;k<=N;k++)
		pthread_join(consumers[k],NULL);
	print_top_ten(main_list);
    return 0;
}