#define BUCKETS (36)
#include<stdio.h>
#include <string.h>
#include<pthread.h>

//basic node structure
typedef struct __node_t {
 int key;
 struct __node_t *next;
} node_t;

//basic list structure
typedef struct __list_t{
	node_t* head;
	pthread_mutex_t lock;
}list_t;

void List_Init(list_t* L){
	L->head=NULL;
	pthread_mutex_init(&L->lock, NULL);
}

void List_Insert(list_t *L, int key) {
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL) {
		printf("unable to allocate memory: malloc FAILED\n" );
		return;
	}
	new->key = key;
	pthread_mutex_lock(&L->lock);
	new->next = L->head;
	L->head = new;
	pthread_mutex_unlock(&L->lock);
	}

int List_Lookup(list_t *L, int key) {
	int rv = -1;
	pthread_mutex_lock(&L->lock);
	node_t *curr = L->head;

	while (curr) {
		if (curr->key == key) {
			rv = 0;
			break;
		}
		curr = curr->next;
	}
	pthread_mutex_unlock(&L->lock);
	return rv;
	}
typedef struct __hash_t{
	list_t lists[BUCKETS];
}hash_t;

void Hash_Init(hash_t* H){
	int i;
	for(i=0;i<BUCKETS;i++){
		List_Init(&H->lists[i]);
	}
}

void Hash_Inset(hash_t* H, int key){
	int bucket=key%BUCKETS;
	return List_Insert(&H->lists[bucket],key);
}

int Hash_Lookup(hash_t* H, int key){
	int bucket=key%BUCKETS;
	return List_Lookup(&H->lists[bucket],key);
}


int main(){
	list_t* myList=(list_t*)malloc(sizeof(list_t));
	List_Init(myList);
	int tmp;
	tmp=List_Lookup(myList,5);
	printf("%d\n",tmp );
	List_Insert(myList,5);
	tmp=List_Lookup(myList,5);
	printf("%d\n",tmp );
	printf("done with lists!!\n");
	hash_t* myHash=(hash_t*)malloc(sizeof(hash_t));
	Hash_Init(myHash);
	tmp=Hash_Lookup(myHash,25);
printf("%d\n",tmp );
	tmp=Hash_Lookup(myHash,26);
printf("%d\n",tmp );
	Hash_Inset(myHash,25);

	tmp=Hash_Lookup(myHash,25);
printf("%d\n",tmp );
	return 0;
}