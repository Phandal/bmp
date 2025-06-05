#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

#define DEFAULT_READ_PATH "res/wall.bmp"
#define DEFAULT_WRITE_PATH "res/test.bmp"

int main(int argc, char **argv) {
  bmp_image_t *pic;
  const char *filepath = DEFAULT_READ_PATH;
  if (argc > 1) {
    filepath = argv[1];
  }
  printf("Loading: %s\n", filepath);

  pic = bmp_load(filepath);
  if (!pic) {
    perror("could not open image");
    exit(EXIT_FAILURE);
  }

  printf("Header:\n");
  printf("pic.header.signature: %s\n", pic->header.signature);
  printf("pic.header.filesize: %u\n", pic->header.filesize);
  printf("pic.header.dataOffset: %u\n", pic->header.dataOffset);
  puts("");

  printf("Info:\n");
  printf("pic.info.size: %u\n", pic->info.size);
  printf("pic.info.width: %u\n", pic->info.width);
  printf("pic.info.height: %u\n", pic->info.height);
  printf("pic.info.planes: %u\n", pic->info.planes);
  printf("pic.info.bitsPerPixel: %u\n", pic->info.bitsPerPixel);
  printf("pic.info.compression: %u\n", pic->info.compression);
  printf("pic.info.imageSize: %u\n", pic->info.imageSize);
  printf("pic.info.xPixelsPerMeter: %u\n", pic->info.xPixelsPerMeter);
  printf("pic.info.yPixelsPerMeter: %u\n", pic->info.yPixelsPerMeter);
  printf("pic.info.colorsUsed: %u\n", pic->info.colorsUsed);
  printf("pic.info.numberOfImportantColors: %u\n",
         pic->info.numberOfImportantColors);
  puts("");

  printf("Color Table:\n\n");

  printf("Pixel: \n");
  printf("pic.numberOfPixels: %lu\n", pic->numberOfPixels);

#ifdef PIXEL_PRINT
  bmp_pixel_t pixel;
  for (unsigned long i = 0; i < pic->numberOfPixels; ++i) {
    pixel = pic->pixel[i];
    printf("R%3u ", pixel.red);
    printf("G%3u ", pixel.green);
    printf("B%3u ", pixel.blue);
    printf("A%3u\t", pixel.alpha);
    printf("#%02X%02X%02X %02X", pixel.red, pixel.green, pixel.blue,
           pixel.alpha);
    printf("\t%lu\n", i);
  }
#else
  printf("\n");
#endif

  if (bmp_save_image(pic, DEFAULT_WRITE_PATH) != 0) {
    perror("could not write file");
    exit(EXIT_FAILURE);
  };
  printf("Wrote test image to: %s\n", DEFAULT_WRITE_PATH);

  bmp_destroy(pic);
  return 0;
}
