#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define HASH_TABLE_LOAD_FACTOR 0.75

#define CSV_MAX_LINE_SIZE 1024
#define CSV_SEPARATOR ','

#define SIZE_OF_HASHTABLE 256

// one line (content) in a hashtable
typedef struct Bucket {
    const char* mKey;
    const void* mValue;
    struct Bucket* mNext;   
} Bucket;

// hash table
typedef struct {
    unsigned int mSize;
    Bucket** mListOfBucket;
} Htable;

//Prototypes

Htable* construct_Htable(size_t size);
void delete_Htable_and_content(Htable*);
void delete_Bucket(Bucket*);
int add_Htable_value(Htable*, const char*,const void*);
int add_Bucket(Htable*, const char*, const void*);
const void* get_Htable_value(Htable*, const char*);
Bucket* get_Htable_bucket(Htable*, const char*);

size_t hash_function(const char* key, size_t size)
{
    size_t hash = 0;
    size_t key_len = strlen(key);
    for (size_t i = 0; i < key_len; ++i) {
        hash += (unsigned char) key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % size;
}


int main (void){
	Htable* table = construct_Htable(SIZE_OF_HASHTABLE);

	if (table != NULL) {
		char* key1 = calloc(10, sizeof(char));
		strcpy(key1, "Suisse");
		char* key2 = calloc(10, sizeof(char));
		strcpy(key2, "France");
		char* key3 = calloc(10, sizeof(char));
		strcpy(key3, "Vietnam");
		char* key4 = calloc(10, sizeof(char));
		strcpy(key4, "China");
		char* key5 = calloc(10, sizeof(char));
		strcpy(key5, "Spain");

		char* value1 = calloc(10, sizeof(char));
		strcpy(value1, "Bern");
		char* value2 = calloc(10, sizeof(char));
		strcpy(value2, "Paris");
		char* value3 = calloc(10, sizeof(char));
		strcpy(value3, "Hanoi");
		char* value4 = calloc(10, sizeof(char));
		strcpy(value4, "Beking");
		char* value5 = calloc(10, sizeof(char));
		strcpy(value5, "Madrid");

		add_Htable_value(table,key1,value1);
		add_Htable_value(table,key2,value2);
		add_Htable_value(table,key3,value3);
		add_Htable_value(table,key4,value4);
		add_Htable_value(table,key5,value5);
	
		printf("%s\n",get_Htable_value(table, key1));
		printf("%s\n",get_Htable_value(table, key2));
		printf("%s\n",get_Htable_value(table, key3));
		printf("%s\n",get_Htable_value(table, key4));
		printf("%s\n",get_Htable_value(table, key5));


		delete_Htable_and_content(table);
	}

	
	

}
/* ****************************************
 * TODO : add your own code here.
 * **************************************** */

Htable* construct_Htable(size_t size){
    if (size < 1)
        return NULL;

    Htable* table = malloc( sizeof(Htable));

    if (table != NULL){
        table->mListOfBucket = calloc(size, sizeof(Bucket*));
        if (table->mListOfBucket == NULL){
            free(table);
            table =  NULL;
        } else {
            int i;
            for(i = 0; i < size; i++){
                table->mListOfBucket[i] = NULL ;
            }
            table->mSize = size;
        }
    }
    // table = NULL si on ne peut pas allouer les mémoires
    return table ;
}

void delete_Htable_and_content(Htable* table){
    size_t size = table->mSize;
    int i;
    for (i = 0; i < size; i++)
        delete_Bucket(table->mListOfBucket[i]);
   	free(table->mListOfBucket);
    table->mListOfBucket = NULL;
    free(table);
    table = NULL;
}

void delete_Bucket(Bucket* bucket){
	if (bucket == NULL)
		return;
	else {
		Bucket* curr;
		while((curr = bucket) != NULL){
			bucket = bucket->mNext;
			free(curr->mValue);
            free(curr->mKey);
			free(curr);
			curr = NULL;
		}
	}
} 


// return 0 si on ne peut pas l'ajouter 1 au contraire
int add_Htable_value(Htable* table, const char* key,const void* value){
    int success;
    Bucket* bucket = get_Htable_bucket(table, key);
    if ( bucket == NULL) { 						//ne pas trouver
        success = add_Bucket(table,key,value);
    } else { 									//juste mis-à-jour sa valeur
        bucket->mValue = value;
        success = 1 ;
    }
    return success;
}

int add_Bucket(Htable* table, const char* key, const void* value){
	Bucket* head = malloc( sizeof(Bucket));

	if(head == NULL)
		return 0;
	else {
		size_t index = hash_function(key, table->mSize);

		head->mKey = key;
		head->mValue = value;
		head->mNext = table->mListOfBucket[index];

		table->mListOfBucket[index] = head ;

		return 1;
	}
}

const void* get_Htable_value(Htable* table, const char* key){
    Bucket* bucket = get_Htable_bucket(table, key);
    if (bucket == NULL)
        return NULL;
    else
        return bucket->mValue; 
}

Bucket* get_Htable_bucket(Htable* table, const char* key){
    Bucket* bucket, *next;
    size_t index = hash_function(key, table->mSize);
    bucket = table->mListOfBucket[index];
    for (; bucket != NULL; bucket = next){
        if (0 == strcmp(key, bucket->mKey))
            return bucket ;
        next = bucket->mNext;
    }
    return NULL;
}
