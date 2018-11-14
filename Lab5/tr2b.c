/* tr2b.c */
#include <stdio.h>
#include <stdlib.h>

////////////////////////////////////
// Forward declarations           //
////////////////////////////////////
void testInput();
int testLength(char const* from, char const* to);
void testDuplicates(char const* from);
void output(char* const from, char* const to);

int main ( int argc, char* argv[]) {

  /* Checking for number of arguments */
  if ( argc != 3 ) {
    fprintf(stderr, "Wrong number of argument inputs. Please give exactly 2 strings.\n");
    exit(1);
  }
  
  /* Testing for stdin inputs */
  testInput();
  
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



///////////////////////////////////////
// Actual implementations            //
///////////////////////////////////////
void testInput() {
  int ch = getchar();
  if ( ch == EOF ) {
    fprintf(stderr, "Please provide a non-empty file to transliterate on.\n");
    exit(1);
  } else {
    ungetc(ch,stdin);  //#
  }
}

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
    fprintf(stderr, "The length of set1 is not equal to the length of set2.\n");
    exit(1);
  }
  return fromCount;
}

void testDuplicates(char const* from) {
  for ( int i = 0 ; from[i] != '\0'; ++i) {
    for ( int j = i + 1; from[j] != '\0'; ++j) {
      if ( from[i] == from[j]) {
         fprintf(stderr, "Duplicate letter in set 1.\n");
         exit(1);
      }
    }
  }
}

void output(char* const from, char* const to) {
  int ch;
  do { 
    ch = getchar();
    if ( ch == EOF ) {
      break;
    }
    int foundMatch = 0;
    for ( int i = 0; from[i] != '\0'; ++i) {
      if ( ch == from[i]) {
        putchar(to[i]);
        foundMatch = 1;
      }
    }
    if ( !foundMatch ) {
      putchar(ch);
    }
  } while ( ch != EOF ); 
}
