#define MAX_UNIQUE_WORDS (1000)
#define MAX_WORD_LENGTH (300)
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <ctype.h>
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
			printf("%s is found, its count is %d\n", str, curr->count );
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
	printf("___________PRINTING LIST_____________\n");
	while(curr){
	printf("%s \t %d\n", curr->word, curr->count);
	curr = curr-> next;		
	}
	printf("___________END OF LIST_____________\n");
}

// int is_alpha (char c) {
// 	if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') return 1;
// 	return 0;
// }
// int is_numeric (char c){
// 	if(isdigit(c)==1)return 1;
// 	return 0;
// }



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
	word_list_t* small_list=(word_list_t*)malloc(sizeof(word_list_t));
	init_word_list(small_list);

	FILE *f;
	char c;
	f=fopen(file_path,"r");
	char curr_word[MAX_WORD_LENGTH];
	// curr_word={};
	// printf("%d\n",strlen(curr_word) );
	while(((c=getc(f))!=EOF))
	{
		// printf("%c", c);
	// 	//do stuff
		int test;
		test = is_alpha_numeric(&c);
		if(test == 1){
			int len=strlen(curr_word);
			curr_word[len]=c;
			curr_word[len+1]='\0';
		}
		else {
			// printf("%s\n",curr_word );
			process_word(curr_word);
			insert_word_in_word_list(small_list, curr_word);
			curr_word[0]='\0';
	// // 		// memset(curr_word,0,strlen(curr_word));
		}
	}
	// printf("the word is::::-->%s\n",curr_word );
	fclose(f);
	// print_word_list(small_list);
	return small_list;
}





int main(){
	word_list_t* my_list=(word_list_t*)malloc(sizeof(word_list_t));
	init_word_list(my_list);
	insert_word_in_word_list(my_list, "Madhu");
	insert_word_in_word_list(my_list, "Vamsi");
	insert_word_in_word_list(my_list, "Madhu");
	// print_word_list(my_list);
	// word_list_t* my_list2=(word_list_t*)malloc(sizeof(word_list_t));
	// init_word_list(my_list2);
	// insert_word_in_word_list_with_count(my_list2, "Kalyan", 2);
	// insert_word_in_word_list(my_list2, "Vamsi");
	// insert_word_in_word_list_with_count(my_list2, "NCSU", 4);
	// print_word_list(my_list2);
	
	// print_word_list(my_list);
	// print_word_list(my_list2);
	word_list_t* gg;
	gg=	file_to_word_list("/home/swat/Desktop/a.zip.txt");
	insert_word_list_in_word_list(my_list, gg);
	print_word_list(my_list);
	// FILE *f;
	// char c;
	// f=fopen("/home/swat/Desktop/a.zip.txt","r");
	// char curr_word[MAX_WORD_LENGTH];
	// // curr_word={};
	// printf("%d\n",strlen(curr_word) );
	// while(((c=getc(f))!=EOF))
	// {
	// 	// printf("%c", c);
	// // 	//do stuff
	// 	int test;
	// 	test = is_alpha_numeric(&c);
	// 	if(test == 1){
	// 		int len=strlen(curr_word);
	// 		curr_word[len]=c;
	// 		curr_word[len+1]='\0';
	// 	}
	// 	else {
	// 		printf("%s\n",curr_word );
	// 		curr_word[0]='\0';
	// // // 		// memset(curr_word,0,strlen(curr_word));
	// 	}
	// }
	// // printf("the word is::::-->%s\n",curr_word );
	// fclose(f);
	return 0;
}
