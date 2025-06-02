#ifndef BMP_H

#define BMP_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 250

typedef struct {
  unsigned char signature[2];
  uint32_t filesize;
  uint32_t dataOffset;
} bmp_header_t;

typedef struct {
  uint32_t size;
  uint32_t width;
  uint32_t height;
  uint16_t planes;
  uint16_t bitsPerPixel;
  uint32_t compression;
  uint32_t imageSize;
  uint32_t xPixelsPerMeter;
  uint32_t yPixelsPerMeter;
  uint32_t colorsUsed;
  uint32_t numberOfImportantColors;
} bmp_info_t;

typedef struct {
  uint8_t blue;
  uint8_t green;
  uint8_t red;
} bmp_pixel_t;

typedef struct {
  bmp_header_t header;
  bmp_info_t info;
  unsigned long numberOfPixels;
  bmp_pixel_t *pixel;
} bmp_image_t;

bmp_image_t *bmp_load(const char *filepath);
void bmp_destroy(bmp_image_t *image);

#endif // BMP_H
