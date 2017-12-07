#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define HASH_TABLE_LOAD_FACTOR 0.75

#define CSV_MAX_LINE_SIZE 1024
#define CSV_SEPARATOR ','

typedef char* csv_row;
typedef const char* csv_const_row;

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

int main (void) {

FILE* f = fopen("/Users/Cescnghia/Desktop/test.txt","r");
int end = 0;

if (f == NULL) {
  fprintf(stderr,"Erreur, on ne peut pas ouvrir le fichier\n");
  fprintf(stderr, "%s\n",strerror(errno));
} else {
    csv_row row = read_row(f); 
    while ((strlen(row) > 0)||end == 1){
        if(end){
          printf("End of File\n");
          end = 0 ;
        }
        else {
          printf("%s\n",row );
          free(row);
          row = read_row(f);
          if(strlen(row) == 0)
            end = 1;
        }
    }


  fclose(f);
}

  return 0;
}