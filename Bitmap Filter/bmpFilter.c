#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

#define BAD_NUMBER_ARGS 1
#define FSEEK_ERROR 2
#define FREAD_ERROR 3
#define MALLOC_ERROR 4
#define FWRITE_ERROR 5

/**
 * Parses the command line.
 *
 * argc:      the number of items on the command line (and length of the
 *            argv array) including the executable
 * argv:      the array of arguments as strings (char* array)
 * grayscale: the integer value is set to TRUE if grayscale output indicated
 *            outherwise FALSE for threshold output
 *
 * returns the input file pointer (FILE*)
 **/

FILE *parseCommandLine(int argc, char **argv, int *isGrayscale) {
  if (argc > 2) {
    printf("Usage: %s [-g]\n", argv[0]);
    exit(BAD_NUMBER_ARGS);
  }

  if (argc == 2 && strcmp(argv[1], "-g") == 0) {
    *isGrayscale = TRUE;
  } else {
    *isGrayscale = FALSE;
  }

  return stdin;
}

unsigned getFileSizeInBytes(FILE* stream) {
  unsigned fileSizeInBytes = 0;

  rewind(stream);
  if (fseek(stream, 0L, SEEK_END) != 0) {
    exit(FSEEK_ERROR);
  }
  fileSizeInBytes = ftell(stream);

  return fileSizeInBytes;
}

void getBmpFileAsBytes(unsigned char* ptr, unsigned fileSizeInBytes, FILE* stream) {
  rewind(stream);
  if (fread(ptr, fileSizeInBytes, 1, stream) != 1) {
#ifdef DEBUG
    printf("feof() = %x\n", feof(stream));
    printf("ferror() = %x\n", ferror(stream));
#endif
    exit(FREAD_ERROR);
  }
}

unsigned char getAverageIntensity(unsigned char blue, unsigned char green, unsigned char red) {
  //printf("TODO: unsigned char getAverageIntensity(unsigned char blue, unsigned char green, unsigned char red)\n");
  int b = blue;
  int g = green;
  int r = red;

  int avg = (b + g + r) / 3;
  return avg;
}

void applyGrayscaleToPixel(unsigned char* pixel) {
  //printf("TODO: void applyGrayscaleToPixel(unsigned char* pixel)\n");
  int avg = getAverageIntensity(*pixel, *(pixel + 1), *(pixel + 2));
  *pixel = avg;
  *(pixel + 1) = avg;
  *(pixel + 2) = avg;
}

void applyThresholdToPixel(unsigned char* pixel) {
  //printf("TODO: void applyThresholdToPixel(unsigned char* pixel)\n");

  int avg = getAverageIntensity(*pixel, *(pixel + 1), *(pixel + 2));
  if (avg < 128) {
    *pixel = 0;
    *(pixel + 1) = 0;
    *(pixel + 2) = 0;
  }
  else {
    *pixel = 255;
    *(pixel + 1) = 255;
    *(pixel + 2) = 255;
  }
}

void applyFilterToPixel(unsigned char* pixel, int isGrayscale) {
  //printf("TODO: void applyFilterToPixel(unsigned char* pixel, int isGrayscale)\n");

  if (isGrayscale == TRUE) {
    applyGrayscaleToPixel(pixel);
  }
  else {
    applyThresholdToPixel(pixel);
  }
}

void applyFilterToRow(unsigned char* row, int width, int isGrayscale) {
  //printf("TODO: void applyFilterToRow(unsigned char* row, int width, int isGrayscale)\n");

  for (int i = 0; i < (width * 3); i += 3) {
    applyFilterToPixel((row + i), isGrayscale);
  }
}

void applyFilterToPixelArray(unsigned char* pixelArray, int width, int height, int isGrayscale) {
  //printf("TODO: void applyFilterToPixelArray(unsigned char* pixelArray, int width, int height, int isGrayscale)\n");

  //(width * 3) = num bytes
  int padding = ((width * 3) % 4);
  if (padding == 1) {
    padding = 3;
  }
  else if (padding == 3) {
    padding = 1;
  }

  int bytesPerRow = (width * 3) + padding;

  for (int i = 0; i < (bytesPerRow * height); i += bytesPerRow) {
    applyFilterToRow((pixelArray + i), width, isGrayscale);
  }
}

void parseHeaderAndApplyFilter(unsigned char* bmpFileAsBytes, int isGrayscale) {
  int offsetFirstBytePixelArray = 0;
  int width = 0;
  int height = 0;
  unsigned char* pixelArray = NULL;

  // set offsetFirstBytePixelArray -> 4 bytes relative to beginning of file
  offsetFirstBytePixelArray = *(bmpFileAsBytes + 10) + *(bmpFileAsBytes + 11) * 256 + *(bmpFileAsBytes + 12) * 256 * 256 + *(bmpFileAsBytes + 13) * 256 * 256 *256;

  // set width -> 4 bytes (signed int)
  width = *(bmpFileAsBytes + 18) + *(bmpFileAsBytes + 19) * 256 + *(bmpFileAsBytes + 20) * 256 * 256 + *(bmpFileAsBytes + 21) * 256 * 256 * 256;

  // set height -> 4 bytes (signed int)
  height = *(bmpFileAsBytes + 22) + *(bmpFileAsBytes + 23) * 256 + *(bmpFileAsBytes + 24) * 256 * 256 + *(bmpFileAsBytes + 25) * 256 * 256 * 256;

  // set the pixelArray to the start of the pixel array
  pixelArray = bmpFileAsBytes + offsetFirstBytePixelArray;

#ifdef DEBUG
  printf("offsetFirstBytePixelArray = %u\n", offsetFirstBytePixelArray);
  printf("width = %u\n", width);
  printf("height = %u\n", height);
  printf("pixelArray = %p\n", pixelArray);
#endif
  applyFilterToPixelArray(pixelArray, width, height, isGrayscale);
}

int main(int argc, char **argv) {
  int grayscale = FALSE;
  unsigned fileSizeInBytes = 0;
  unsigned char* bmpFileAsBytes = NULL;
  FILE *stream = NULL;

  stream = parseCommandLine(argc, argv, &grayscale);
  fileSizeInBytes = getFileSizeInBytes(stream);

#ifdef DEBUG
  printf("fileSizeInBytes = %u\n", fileSizeInBytes);
#endif

  bmpFileAsBytes = (unsigned char *)malloc(fileSizeInBytes);
  if (bmpFileAsBytes == NULL) {
    exit(MALLOC_ERROR);
  }
  getBmpFileAsBytes(bmpFileAsBytes, fileSizeInBytes, stream);

  parseHeaderAndApplyFilter(bmpFileAsBytes, grayscale);

#ifndef DEBUG
  if (fwrite(bmpFileAsBytes, fileSizeInBytes, 1, stdout) != 1) {
    exit(FWRITE_ERROR);
  }
#endif

  free(bmpFileAsBytes);
  return 0;
}
