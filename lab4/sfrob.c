#include <stdio.h>
#include <stdlib.h>

//////////////////////////
// Constants            //
//////////////////////////

const int KEY = 42;

//////////////////////////
// Forward decalartions //
//////////////////////////
char unfrob ( const char* a);
int frobcmp ( const char* a, const char* b);
int frobcmpWrapper ( const void* a, const void* b);
void testInput();
void* myRealloc(void* pointer, int size);
char* parseAnEntry(FILE* fp);
int mallocInput(FILE* fp, char*** arrayPointer);
void printAll(char** array, const int size);
void freeAll(char** array, const int size);

int main () {
  
  /* Testing input */
  testInput();
  
  /*Intialize pointers on stack.*/
  char** array;
  char*** arrayPointer = &array;
  
  /*Malloc input */
  int size = mallocInput(stdin, arrayPointer);
  
  /*Checking array*/
  if ( array == NULL ) {
    fprintf(stderr,"Mallocing Input Failed. \n");
    exit(1);
   }
  
  /*Creating function pointer and qsorting data */
  int (*cmpWrapper) ( const void* a, const void* b);
  cmpWrapper = & frobcmpWrapper;
  qsort(array,size,sizeof(char**), cmpWrapper);

  /*Printing and Freeing*/
  char** arrayCopy = array;
  printAll(arrayCopy,size);
  freeAll(array,size);
  
}









//////////////////////////
//Actual Implementations//
//////////////////////////
char unfrob ( const char* a) {
  char result = *a;
  result ^= KEY;
  return result;
}

int frobcmp( const char* a, const char* b) {
  while ( *a != ' ' && *b != ' ') {
    char left = unfrob(a);
    char right = unfrob(b);
    if ( left < right ) {
      return -1;
    } else if ( left > right) {
      return 1;
    }
    a++;
    b++;
  }
  
  /* 
    Exiting the while loop, 
    case 1: equivalence situation
    case 2: prefix situation
   */
   if ( *a == ' ' && *b == ' ') {
     return 0;
   } else if ( *a == ' ') {
     return -1;
   }
     return 1;
}

int frobcmpWrapper( const void* a, const void* b) {
  char** left = (char**) a;
  char** right = (char**) b;
  return frobcmp(*left, *right);
}

void testInput() {
  int ch = getc(stdin);
  if ( feof (stdin) != 0 ) {
    printf(" ");
    exit(0);
  } else {
    ungetc(ch,stdin);
  }
}

void* myRealloc(void* pointer, int size) {
  void* newPointer = realloc(pointer,size);
  if ( newPointer == NULL) {
    fprintf(stderr, "Reallocation failed.\n");
    exit(1);
  }
  return newPointer;
}

char* parseAnEntry(FILE* fp) {
  
  /* Test upon Entry */
  if ( ferror(fp) != 0 ) {
    fprintf(stderr, "File error is set.\n");
    exit(1);
  }

  /*Test2 upon Entry: Only the EOF left*/
  int test = getc(fp);
  if ( test == EOF ) {
    return NULL;
  } else {
    ungetc(test,fp);
  }
  
  int capacity = 5;
  int counter = 0;
  char* headOfWord = (char*) malloc(capacity*sizeof(char));
  
  /*Test upon dereferencing*/
  if ( headOfWord == NULL ) {
    fprintf(stderr, "Malloc failed.\n");
    exit(1);
  }
  
  /*Making a copy of headOfWord to manipulate*/
  char* headCopy = headOfWord;
  
  int ch; 
  
  /*de-duplication of spaces in the beginning of an entry*/
  ch = getc(fp);
  while ( ch != EOF && ( ch == ' ' || ch == '\n')) {
    //trailing delimters
    if (ferror(fp) != 0) {
      fprintf(stderr, "File error is set.\n");
      exit(1);
    }
    ch = getc(fp);
  }
  
  /*if the remaining of entries/file has delimiters only*/
  if ( ch == EOF ) {
    free(headOfWord);
    return NULL;
  }

  /*when encountering the first non-space byte*/
  while ( ch != EOF && (ch != ' ' && ch != '\n')) {
    if ( capacity <= counter + 2 ) {
      capacity *= 2;
      headOfWord = myRealloc(headOfWord,capacity*sizeof(char));
      headCopy = headOfWord + counter;
    }
    *headCopy = ch;
    ch = getc(fp);
    headCopy++;
    counter++;
  }
 
  /*appending the trailing spaces*/
  if (counter != 0 && *(headCopy-1) != ' ') {
    if ( capacity <= counter + 2 ) {
      capacity *= 2;
      headOfWord = myRealloc(headOfWord,capacity*sizeof(char));
      headCopy = headOfWord + counter;
    }
    *headCopy = ' ';
  }
  return headOfWord;
}

int mallocInput(FILE* fp, char*** arrayPointer) {

  /* Test upon Entry */
  if ( ferror(fp) != 0 ) {
    fprintf(stderr, "File error is set.\n");
    exit(1);
  }
  
  int capacity = 4;
  int counter = 0;
  *arrayPointer = (char**) malloc(capacity*sizeof(char**));
  
  /*Test upon dereferencing*/
  if ( *arrayPointer == NULL ) {
    fprintf(stderr, "Malloc failed.\n");
    exit(1);
  }
  
  /*Making a copy of arrayPointer to manipulate*/
  char** headCopy = *arrayPointer;
  
  while ( !feof(fp) ) {
    if (capacity <= counter + 2 ) {
      capacity *= 2;
      *arrayPointer = myRealloc(*arrayPointer,capacity*sizeof(char**));
      headCopy = (*arrayPointer) + counter;
    }
    *headCopy = parseAnEntry(fp);
    if ( *headCopy == NULL) { //not an Entry
      break;
    }
    headCopy++;
    counter++;
  }
  return counter;
}

void printAll(char** array, const int size) {
  int counter = 0;
  while ( counter < size) {
    char* temp = *array;
    char c = 'a';
    do {
      c = *temp;
      printf("%c",c);
      temp++;
    } while ( c != ' ');
    counter++;
    array++;
  }
}

void freeAll(char** array, const int size) {
  char** copy = array;
  for ( int i = 0; i < size; i++) {
    free(*copy);
    copy++;
  }
  free(array);
}
