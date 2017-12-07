// C99

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define HASH_TABLE_LOAD_FACTOR 0.75
#define CSV_MAX_LINE_SIZE 1024
#define CSV_SEPARATOR ','


// Bucket
typedef struct Bucket{
    const char* mKey;
    const void* mValue;
    struct Bucket* mNext;    
} Bucket;

// Hash table
typedef struct {
    unsigned int mSize;
    Bucket** mListOfBucket;
} Htable;


typedef char* csv_row;
typedef const char* csv_const_row;

//Prototypes

Htable* construct_Htable(size_t size);
void delete_Htable_and_content(Htable*);
void delete_Bucket(Bucket*);
int add_Htable_value(Htable*, const char*,const void*);
int add_Bucket(Htable*, const char*, const void*);
const void* get_Htable_value(Htable*, const char*);
Bucket* get_Htable_bucket(Htable*, const char*);

int add_row_to_hashtable(Htable*, csv_row, size_t);
int hash_join(FILE*, FILE*, FILE*, size_t, size_t, size_t);
void join(Htable*, FILE*, FILE*, size_t);

 

/* ======================================================================
 * Provided utility functions
 * ======================================================================
 */

/** ----------------------------------------------------------------------
 ** Ask for a filename
 **/
void ask_filename(const char* question, char filename[])
{
    size_t len = 0;
    do {
        fputs(question, stdout);
        fflush(stdout);
        fgets(filename, FILENAME_MAX + 1, stdin);
        filename[strcspn(filename, "\r\n")] = '\0';
        len = strlen(filename) - 1;
    } while (len < 1 && !feof(stdin) && !ferror(stdin));
}

/** ----------------------------------------------------------------------
 ** Ask for a filename to open in a given mode
 **/
FILE* ask_filename_and_open(const char* question, const char* mode)
{
    char filename[FILENAME_MAX + 1] = "";

    ask_filename(question, filename);
    FILE* retour = fopen(filename, mode);

    if (retour == NULL) {
        fprintf(stderr, "Oops, je n'ai pas réussi à ouvrir le fichier \"%s\" en mode \"%s\"!\n",
                filename, mode);
    }
    return retour;
}

/** ----------------------------------------------------------------------
 ** Ask for a size
 **/
size_t ask_size_t(const char* question)
{
    size_t v = 0;
    int res = 0;
    do {
        fputs(question, stdout);
        fflush(stdout);
        res = scanf("%zu", & v);
        while (!feof(stdin) && !ferror(stdin) && getc(stdin) != '\n');
    } while (!feof(stdin) && !ferror(stdin) && res != 1 && v == 0);
    return v;
}

/* ======================================================================
 * Part I -- Hashtable
 * ======================================================================
 */

/** ----------------------------------------------------------------------
 ** Hash a string for a given hashtable size.
 ** See http://en.wikipedia.org/wiki/Jenkins_hash_function
 **/
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

/* ****************************************
 * TODO : add your own code here.
 * **************************************** */

// fonction pour construire un hash table avec sa taille
// return NULL si on n'arrive pas à allouer 

Htable* construct_Htable(size_t size){
    if (size < 1)
        return NULL;

    Htable* table = malloc( sizeof(Htable));

    if (table != NULL) {
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
    return table;
}

// fonction pour détruire un hash table

void delete_Htable_and_content(Htable* table){
    size_t size = table->mSize;
    int i;
    for(i = 0; i < size; i++)
        delete_Bucket(table->mListOfBucket[i]);

    free(table->mListOfBucket);
    table->mListOfBucket = NULL;

    free(table);
    table = NULL;
}

// fonction pour détruire un bucket

void delete_Bucket(Bucket* bucket){
    if (bucket == NULL)
        return;
    else{
        Bucket* curr;
        while((curr = bucket) != NULL){
            bucket = bucket->mNext;
            free((void*)curr->mValue);  //cash void* pour éviter le warning
            free((void*)curr->mKey);
            free(curr);
            curr = NULL;
        }
    }
}

// fonction pour ajouter un key et une valeur dans le hash table
// return 0 si réussit

int add_Htable_value(Htable* table, const char* key,const void* value){
    int success;
    Bucket* bucket = get_Htable_bucket(table, key);
    if ( bucket == NULL) {                      //ne pas trouver => ajoute nouveau bucket
        success = add_Bucket(table,key,value);
    } else {                                    //trouver => mettre à jour sa valeur
        bucket->mValue = value;
        success = 0 ;
    }
    return success;
}

// fonction pour ajouter un nouveau bucket 
// return 0 si réussit

int add_Bucket(Htable* table, const char* key, const void* value){
    Bucket* head = malloc( sizeof(Bucket));

    if (head == NULL)
        return -1;
    else {
        size_t index = hash_function(key, table->mSize);
        head->mKey = key;
        head->mValue = value;
        head->mNext = table->mListOfBucket[index];
        table->mListOfBucket[index] = head ;
        return 0;
    }
}

// fonction pour récupérer la valeur à partir d'un key
// return NULL si le key n'existe pas

const void* get_Htable_value(Htable* table, const char* key){
    Bucket* bucket = get_Htable_bucket(table, key);
    if (bucket == NULL)
        return NULL;
    else
        return bucket->mValue; 
}

//fonction pour récupérer le bucket à partir d'un key
// return NULL si le key n'existe pas

Bucket* get_Htable_bucket(Htable* table, const char*  key){
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

/* ======================================================================
 * Provided: CSV file parser
 * ======================================================================
 */


/** ----------------------------------------------------------------------
 ** Allocate and read a CSV row from a file
 **/
csv_row read_row(FILE* f)
{
    char line[CSV_MAX_LINE_SIZE + 1] = "";
    fgets(line, CSV_MAX_LINE_SIZE, f);
    line[strcspn(line, "\r\n")] = '\0'; // remove trailing '\n'
    size_t len = strlen(line);

    assert(len < CSV_MAX_LINE_SIZE); /* We should not have lines that long...
                                      * Not handled in this homework anyway!
                                      * Should be properly handled in real-life appli.
                                      */

    csv_row row;
    if ((row = calloc(len + 1, sizeof(char))) == NULL) {
        return NULL;
    }
    strncpy(row, line, len);
    return row;
}

/** ----------------------------------------------------------------------
 ** Write a CSV row to a file
 **/
void write_row(FILE* out, const csv_const_row row, size_t ignore_index)
{
    size_t len = strlen(row);
    size_t current_element = 0;
    for (size_t i = 0; i < len; ++i) {
        if (row[i] == CSV_SEPARATOR) {
            ++current_element;
        }
        if (current_element != ignore_index && (ignore_index != 0 || current_element != ignore_index + 1 || row[i] != CSV_SEPARATOR)) {
            fprintf(out, "%c", row[i]);
        }
    }
}

/** ----------------------------------------------------------------------
 ** Write 2 CSV rows side-by-side to a file
 **/
void write_rows(FILE* out, const csv_const_row row1, const csv_const_row row2, size_t ignore_index)
{
    write_row(out, row1, (size_t) -1);
    fprintf(out, "%c", CSV_SEPARATOR);
    write_row(out, row2, ignore_index);
    fprintf(out, "\n");
}

/** ----------------------------------------------------------------------
 ** Copy and return the i'th element in the row
 **/
char* row_element(const csv_const_row row, size_t index)
{
    size_t len = strlen(row);
    size_t start = 0, end = 0;
    size_t current_element = 0;
    for (size_t i = 0; i < len; ++i) {
        if (row[i] == CSV_SEPARATOR || i == len - 1) {
            ++current_element;
            if (current_element == index) {
                start = i + 1;
            } else if (current_element == index + 1) {
                end = (i == len - 1) ? len : i;
                break;
            }
        }
    }

    if (end > 0) { // success
        size_t elem_len = end - start;
        char* element;
        if ((element = calloc(elem_len + 1, sizeof(char))) == NULL) {
            return NULL;
        }
        element[elem_len] = '\0';
        strncpy(element, &row[start], elem_len);
        return element;
    } else {
        return NULL;
    }
}


/* ======================================================================
 * Part II -- Hashjoin
 * ======================================================================
 */

/* ****************************************
 * TODO : add your own code here.
 * **************************************** */

// fonction pour faire le hash-join
// return O si réussit

int hash_join(FILE* in1, FILE* in2, FILE* out, size_t col1, size_t col2, size_t size_memory){
    size_t size_of_htable = size_memory / sizeof(Bucket*);
    size_t full = HASH_TABLE_LOAD_FACTOR * size_of_htable;
    Htable* table = construct_Htable(size_of_htable);

    if (table == NULL){
        fprintf(stderr, "On ne peut pas construire un hash table\n");
        return -1;
    }

    csv_row rowR1;
    write_rows(out, read_row(in1), read_row(in2), col2); // écrire en-tete
    int count = 0; // nombre d'élement dans le hash table, pour comparer avec "full"

    while(strlen(rowR1 = read_row(in1)) > 0) {

        if (0 == add_row_to_hashtable(table, rowR1, col1)) {
            count++ ;
        } else { // on ne peut pas ajouter R1 dans hash table => goto fail
            fprintf(stderr, "On ne peut pas ajouter R1 dans hash table\n");
            return -1;
        }
        // hash table est pleine => join
        if (count==full){ 
            join(table, in2, out, col2);
            count = 0;
        }
    }

    if (count != 0) // R1 a été entièrement scannée, si hash table n'est pas vide, on fait join encore une fois
        join(table, in2, out, col2);

    delete_Htable_and_content(table);
    return 0;
}

// fonction qui lit R2 et écrire le résultat dans "out"

void join(Htable* table, FILE* in, FILE* out, size_t col){
    fseek(in, 0, SEEK_SET);
    csv_row row = read_row(in); //ignore header

    while(strlen(row = read_row(in)) > 0){
        char* key = row_element(row, col);
        csv_const_row value = get_Htable_value(table, key);
        if (value != NULL)
            write_rows(out, value, row, col);
        free(key);
    }
    // remis hash table à zero 
    size_t size = table->mSize;
    delete_Htable_and_content(table);
    table = construct_Htable(size);
    if (table == NULL)
        fprintf(stderr, "On ne peut pas remettre hash table à zero.\n");
}

// fonction pour ajouter une ligne csv dans le hash table
// return 0 si réussit 

int add_row_to_hashtable(Htable* table, csv_row row, size_t col){
    int success;
    char* key = row_element(row, col);
    if (key != NULL)
        success = add_Htable_value(table, key, row);    
    else
        success = -1;
    return success;
}

/* ======================================================================
 * Provided: main()
 * ======================================================================
 */

int main(void)
{
    FILE* in1 = ask_filename_and_open("Entrez le nom du premier fichier : ", "r");
    if (in1 == NULL) {
        return EXIT_FAILURE;
    }

    FILE* in2 = ask_filename_and_open("Entrez le nom du second  fichier : ", "r");
    if (in2 == NULL) {
        fclose(in1);
        return EXIT_FAILURE;
    }

    FILE* out = ask_filename_and_open("Entrez le nom du fichier où écrire le résultat : ", "w");
    if (out == NULL) {
        fclose(in1);
        fclose(in2);
        return EXIT_FAILURE;
    }

    size_t col1 = ask_size_t("Entrez l'index de la colonne à joindre dans le premier fichier : "),
           col2 = ask_size_t("Entrez l'index de la colonne à joindre dans le second  fichier : ");

    int success = hash_join(in1, in2, out, col1, col2,
                            ask_size_t("Entrez le budget mémoire autorisé (en octets) : ")
                            );

    fclose(in1);
    fclose(in2);
    fclose(out);

    return success;
}
