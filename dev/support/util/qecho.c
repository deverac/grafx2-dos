#include <stdio.h>
#include <string.h>

// This is a simple program to echo text in order to work aronud a problem
// with FreeDOS's 'echo' command. If quoted text is supplied to FreeDOS's
// 'echo' command, the quotes will be printed. This program will not output
// surrounding quotes.

#define DQUOTE '"'

int main (int argc, char **argv) {
  int len;
  int i;
  char* first_ch;
  char* last_ch;

  if (argc == 1) {
     return 0;
  }

  for (i=1; i<argc; i++) {
  		len = strlen(argv[i]);
		
  		first_ch = argv[i];
  		last_ch = (first_ch + len - 1); 
				
  		if ((*first_ch == DQUOTE) && (*last_ch != DQUOTE)) {
          		fprintf(stderr, "Arg %d missing ending quote.\n", i);
          		return 1;
  		}
		
  		if ((*first_ch != DQUOTE) && (*last_ch == DQUOTE)) {
          		fprintf(stderr, "Arg %d missing starting quote.\n", i);
          		return 1;
  		}
				
  		if ((*first_ch == DQUOTE) && (*last_ch == DQUOTE)) {
      		first_ch++;
      		*last_ch = '\0';
  		}
		
  		while (first_ch <= last_ch) {
      		printf("%c", *first_ch);
      		first_ch++;
  		}
  }
  printf("\n");

  return 0;
}
