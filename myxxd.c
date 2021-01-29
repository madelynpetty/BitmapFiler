#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define BAD_NUMBER_ARGS 1

/**
 * Parses the command line.
 *
 * argc: the number of items on the command line (and length of the
 *       argv array) including the executable
 * argv: the array of arguments as strings (char* array)
 * bits: the integer value is set to TRUE if bits output indicated
 *       outherwise FALSE for hex output
 *
 * returns the input file pointer (FILE*)
 **/
FILE *parseCommandLine(int argc, char **argv, int *bits) {
  if (argc > 2) {
    printf("Usage: %s [-b|-bits]\n", argv[0]);
    exit(BAD_NUMBER_ARGS);
  }

  if (argc == 2 &&
      (strcmp(argv[1], "-b") == 0 || strcmp(argv[1], "-bits") == 0)) {
    *bits = TRUE;
  } else {
    *bits = FALSE;
  }

  return stdin;
}

/**
 * Writes data to stdout in hexadecimal.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsHex(unsigned char *data, size_t size) { 
   //printf("TODO 1: printDataAsHex (2)");

  for (int i = 0; i < size; i++) {
    printf("%02x", data[i]);

    if (i % 2 == 1) {
      printf(" ");
    }
  }

  if (size == 1) {
      printf("                                      "); //38
    }
    else if (size == 2) {
      printf("                                   "); //35
    }
    else if (size == 3) {
      printf("                                 "); //33
    }
    else if (size == 4) {
      printf("                              "); //30
    }
    else if (size == 5) {
      printf("                           "); //28
    }
    else if (size == 6) {
      printf("                         "); //26
    }
    else if (size == 7) {
      printf("                       "); //23
    }
    else if (size == 8) {
      printf("                    "); //20
    }
    else if (size == 9) {
      printf("                  "); //18
    }
    else if (size == 10) {
      printf("               "); //15
    }
    else if (size == 11) {
      printf("             "); //13
    }
    else if (size == 12) {
      printf("          "); //10
    }
    else if (size == 13) {
      printf("        "); //8
    }
    else if (size == 14) {
      printf("     "); //5
    }
    else if (size == 15) {
      printf("   "); //3
    }

}

/**
 * Writes data to stdout as characters.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsChars(unsigned char *data, size_t size) {
  //printf("TODO 2: printDataAsChars (3)");

  for (int i = 0; i < size; i++) {
     if (data[i] >= 32 && data[i] <= 126) {
       printf("%c", data[i]);
     }
     else {
       printf(".");
     }
  }

}

void readAndPrintInputAsHex(FILE *input) {
  unsigned char data[16];
  int numBytesRead = fread(data, 1, 16, input);
  //int padding = (16 - numBytesRead) * 5;
  unsigned int offset = 0;

  while (numBytesRead != 0) {
    printf("%08x: ", offset);
    offset += numBytesRead;
    printDataAsHex(data, numBytesRead);
    printf(" ");
    printDataAsChars(data, numBytesRead);
    printf("\n");
    numBytesRead = fread(data, 1, 16, input);
  }
}

void printDataAsBits(unsigned char* data, size_t size) {
  unsigned int bin[8];
  for (int i = 0; i < size; i++) {
    int x = data[i];
    for (int j = 0; j < 8; j++) {
      bin[j] = (x % 2);
      x = x / 2;
    }
    for (int m = 7; m >= 0; m--) {
      printf("%d", bin[m]);
    }
    printf(" ");
  }

  if (size < 6) {
    for (int i = 0; i < (6 - size); i++) {
      printf("         ");
    }
  }

}

/**
 * Bits output for xxd.
 *
 * See myxxd.md for details.
 *
 * input: input stream
 **/
void readAndPrintInputAsBits(FILE *input) {
  //printf("TODO 3: readAndPrintInputAsBits\n");
  unsigned char data[6];
  int numBytesRead = fread(data, 1, 6, input);
  unsigned int offset = 0;
  while (numBytesRead != 0) {
    printf("%08x: ", offset);
    offset += numBytesRead;
    printDataAsBits(data, numBytesRead);
    printf(" ");
    printDataAsChars(data, numBytesRead);
    printf("\n");
    numBytesRead = fread(data, 1, 6, input);
  }
}

int main(int argc, char **argv) {
  int bits = FALSE;
  FILE *input = parseCommandLine(argc, argv, &bits);

  if (bits == FALSE) {
    readAndPrintInputAsHex(input);
  } else {
    readAndPrintInputAsBits(input);
  }
  return 0;
}
