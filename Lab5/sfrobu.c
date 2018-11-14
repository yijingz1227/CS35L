#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

//////////////////////////////
//  CONSTANTS               //
//////////////////////////////

const int KEY = 42;
const int STANDARDINPUT = 0;
const int STANDARDOUTPUT = 1;
const int REGULARFILE = 6;
const int PIPELINE = 7;
const int FINVALIDINPUT = 94;
const int OPTIONS = 95;
const int REALLOCFAILURE = 96;
const int MALLOCFAILURE = 97;
const int READFAILURE = 98;
const int INVALIDINPUT = 99;

//////////////////////////////
// Forward declarations     //
//////////////////////////////
void errorMsgCenter(int errorNumber);
int checkInputStatus(int fd, struct stat* buf);
int allocRegFile(int fd, struct stat* buf, char** headPointer);
int parseRegFile(char* const head, char*** arrayPointer, int size);
void* myRealloc(void* pointer, int size);
char unfrob ( const char* a);
int frobcmp ( const char* a, const char* b);
int frobcmp_F (const char* a, const char* b);
int frobcmpWrapper ( const void* a, const void* b);
int frobcmpWrapper_F (const void* a, const void* b);
void printAll(char** array, const int size);

int main ( int argc, char* argv[]) {
  
  struct stat status;
  int fileType = checkInputStatus(STANDARDINPUT, &status);

  int size = 0;
  char** array;
  char*** arrayPointer = &array;
  char* head;
  char** headPointer = &head;

  
  int upFlag = 0;
  if ( argc > 2 ) {
    errorMsgCenter(OPTIONS);
    exit(1);
  }

  /*Testing if it is -f option*/
  char* option = argv[1];
   if (argc == 2 && option[0] == '-' && option[1] == 'f' && option[2] == '\0') {
     upFlag = 1;
   } else if (argc == 2) {
     errorMsgCenter(OPTIONS);
     exit(1);
   }
  
  if ( fileType == REGULARFILE || fileType == PIPELINE ) {
    int newSize  = allocRegFile(STANDARDINPUT, &status, headPointer);
    size = parseRegFile(head,arrayPointer, newSize); 
  }

  /*Creating function pointer and qsorting data */
  int (*cmpWrapper) ( const void* a, const void* b);
  if ( upFlag ) {
    cmpWrapper = & frobcmpWrapper_F;
  } else {
    cmpWrapper = & frobcmpWrapper;
  }
  qsort(array,size,sizeof(char**), cmpWrapper);

  char** arrayCopy = array;
  printAll(arrayCopy, size);

  /* Freeing allocated space */
  free(head);
  free(array);
  
}






////////////////////////////////
//Actual Implementations      //
////////////////////////////////
void errorMsgCenter( int errorNumber) {
  char defaultErr[40] = "Invalid error number.\n";
  char invalidInput[100] = "Invalid standard input. Neither a regular file nor a proper pipeline.\n";
  char readError[20] = "Read failed.\n";
  char mallocError[20] = "Malloc failed.\n";
  char reallocError[20] = "Realloc failed.\n";
  char optionsError[40] = "Too many options or wrong options.\n";
  char fInvalidInput[40] = "Character value out of range.\n";
  char* errorPointer;
  switch(errorNumber) {
    case 99: 
        errorPointer = invalidInput;
	break;
    case 98:
        errorPointer = readError;
	break;
    case 97:
         errorPointer = mallocError;
         break;
    case 96:
        errorPointer = reallocError;
        break;
    case 95:
        errorPointer = optionsError;
        break;
    case 94:
        errorPointer = fInvalidInput;
        break;
    default:
        errorPointer = defaultErr;
	break;
  }
  for ( int i = 0; errorPointer[i] != '\0'; ++i) {
    write(2,errorPointer+i,1);
  }
}

int checkInputStatus(int fd, struct stat* buf) {
  if ( fstat(fd, buf) == -1) {
    errorMsgCenter(INVALIDINPUT);
    exit(1);
  }
  
  if (S_ISREG(buf->st_mode)) {
    return REGULARFILE;
  } else if (S_ISFIFO(buf->st_mode)) {
    return PIPELINE;
  }
   
  return INVALIDINPUT;
}

int allocRegFile(int fd, struct stat* buf, char** headPointer) {
  int size = buf->st_size;
  int initialCapacity = size + 1;
  
  *headPointer = (char*)  malloc(initialCapacity*sizeof(char));
  if ( size != 0 ) {
    size = read(fd, *headPointer, size);
  }
  if ( size == -1) {
    errorMsgCenter(READFAILURE);
    exit(1);
  }
   
  int flag;

  do {
    if ( initialCapacity <= size + 2) {
      initialCapacity *= 2;
      *headPointer = myRealloc(*headPointer, initialCapacity * sizeof(char));
    }
    flag = read(fd,*headPointer + size,sizeof(char));
    if ( flag == -1) {
      errorMsgCenter(READFAILURE);
      exit(1);
    }
    size += flag;
  } while ( flag != 0);

  char* head = *headPointer;
  /* Appending the space if there is no trailing space at the end of the regular file. */
  if ( head[size - 1] != ' ') {
    head[size] = ' ';
    size++;
   }
   
   return size;
}

int parseRegFile(char* const head, char*** arrayPointer,int size) {
  char* headCopy = head;
  if ( size == 0 ) {
    return 0;
  }
  
  int count = 0;
  int capacity = 3;
  *arrayPointer = (char**) malloc ( capacity * sizeof (char**));
  char** array = *arrayPointer;
  
  /*Test upon dereferencing*/
  if ( *arrayPointer == NULL ) {
    errorMsgCenter(MALLOCFAILURE);
    exit(1);
  }
  
  for (int i = 0 ; i < size; ++i) {
    if ( capacity <= count + 2) {
      capacity *= 2;
      *arrayPointer = myRealloc(*arrayPointer, capacity*sizeof(char**));
      array = (*arrayPointer) + count;
    }
    if (headCopy[i] != ' ' && ( i == 0 || head[i-1] == ' ')) {
      *array = headCopy+i;
      array++;
      count++;
    } 
  }
  return count;
}

void* myRealloc(void* pointer, int size) {
  void* newPointer = realloc(pointer,size);
  if ( newPointer == NULL) {
    errorMsgCenter(REALLOCFAILURE);
    exit(1);
  }
  return newPointer;
}

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

int frobcmp_F( const char* a, const char* b) {
  while ( *a != ' ' && *b != ' ') {
    char left = unfrob(a);
    char right = unfrob(b);
    if ( left < -1 || right < -1 ) {
      errorMsgCenter(FINVALIDINPUT);
      exit(1);
    } else {
      left = toupper(left);
      right = toupper(right);
    }
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

int frobcmpWrapper_F (const void* a, const void* b) {
   char** left = (char**) a;
   char** right = (char**) b;
   return frobcmp_F(*left, *right);

}

void printAll(char** array, const int size) {
  int counter = 0;
  while ( counter < size) {
    char* temp = *array;
    char c = 'a';
    do {
      c = *temp;
      write(1, temp, 1);
      temp++;
    } while ( c != ' ');
    counter++;
    array++;
  }
}
