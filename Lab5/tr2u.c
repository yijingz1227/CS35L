/*tr2u.c*/
#include <stdlib.h>
#include <unistd.h>

///////////////////////////////
// Forward declarations      //
///////////////////////////////
int testLength(char const* from, char const* to);
void testDuplicates(char const* from);
void output( char* const from, char* const to);


int main(int argc, char* argv[]) {
  if ( argc != 3 ) {
    char errorMsg[100] = "Wrong number of argument inputs. Please give exactly 2 strings.\n";
    for ( int i = 0; errorMsg[i] != '\0'; ++i) {
      write(2, errorMsg+i,1);
    }
    exit(1);
  }  
  
  /*Parsing 'from' and 'to' strings */
  char* from = argv[1];
  char* from1 = argv[1];
  char* to = argv[2];
  
  /*Testing length of from and to */
  int length = testLength(from,to);
  
  /*Testing duplicates in 'from' strings */
  testDuplicates(from1);
  
  /*print transliterate strings*/
  output(from,to);
}


////////////////////////////////
// Actual Implementations     //
////////////////////////////////

int testLength(char const* from, char const* to) {
  int fromCount = 0;
  for ( int i = 0; from[i] != '\0'; i++) {
    fromCount++;
  }
  int toCount = 0;
  for ( int j = 0 ; to[j] != '\0'; j++) {
    toCount++;
  }
  if (fromCount != toCount) {
    char errorMsg[100] ="The length of set1 is not equal to the length of set2.\n";
    for ( int i = 0; errorMsg[i] != '\0'; ++i) {
      write(2, errorMsg+i, 1);
    }
    exit(1);
  }
  return fromCount;
}

void testDuplicates(char const* from) {
  for ( int i = 0 ; from[i] != '\0'; ++i) {
    for ( int j = i + 1; from[j] != '\0'; ++j) {
      if ( from[i] == from[j]) {
	char errorMsg [100] =  "Duplicate letter in set 1.\n";
	for ( int i = 0; errorMsg[i] != '\0'; ++i) {
	  write(2,errorMsg+i, 1);
	}
        exit(1);
      }
    }
  }
}

void output(char* const from, char* const to) {
  char buf[1];
  int fd_in = 0;
  int fd_out = 1;
  int readAmount = 1;
  ssize_t bytes_read;
  
  do { 
    bytes_read = read(fd_in, buf, readAmount);
    if ( bytes_read == 0 ) {
      break;
    }
    int foundMatch = 0;
    for ( int i = 0; from[i] != '\0'; ++i) {
      if ( buf[0] == from[i]) {
        write(fd_out, to+i, 1);
        foundMatch = 1;
      }
    }
    if ( !foundMatch ) {
      write(fd_out,buf,1);
    }
  } while ( bytes_read != 0 ); 
}



