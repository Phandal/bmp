#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_FILE_PATH "res/wall.bmp"
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

int _bmp_read_string(uint8_t *dest, unsigned int n, uint8_t **buffer) {
  memmove(dest, *buffer, n);
  *buffer += n;
  return n;
}

int _bmp_read(void *dest, uint8_t **buffer, unsigned int n) {
  int i = 0;
  unsigned long readNumber = 0;

  if (dest == NULL) {
    // Just read to advance over reserved bytes
    *buffer += n;
    return n;
  }

  for (i = n - 1; i >= 0; --i) {
    readNumber = (readNumber << 8) | (*buffer)[i];
  }
  (*buffer) += n;
  memmove(dest, &readNumber, n);

  return n;
}

int _bmp_read_uint8(uint8_t *dest, uint8_t **buffer) {
  return _bmp_read(dest, buffer, 1);
}

int _bmp_read_uint16(uint16_t *dest, uint8_t **buffer) {
  return _bmp_read(dest, buffer, 2);
}

int _bmp_read_uint32(uint32_t *dest, uint8_t **buffer) {
  return _bmp_read(dest, buffer, 4);
}

FILE *bmp_open_file(const char *filepath, unsigned long *fileSize) {
  FILE *imageFile;
  *fileSize = 0;

  imageFile = fopen(filepath, "rb");
  if (!imageFile) {
    return NULL;
  }

  if (fseek(imageFile, 0, SEEK_END) == -1) {
    fclose(imageFile);
    return NULL;
  }

  *fileSize = ftell(imageFile);
  if (*fileSize == -1) {
    fclose(imageFile);
    return NULL;
  }

  rewind(imageFile);

  return imageFile;
}

bmp_image_t *bmp_load(const char *filepath) {
  FILE *imageFile;
  unsigned long bufferSize;
  uint8_t *buffer;
  unsigned long readLength;
  unsigned long offset;

  bmp_image_t *image = calloc(1, sizeof(bmp_image_t));
  if (!image) {
    return NULL;
  }

  imageFile = bmp_open_file(filepath, &bufferSize);
  if (!imageFile) {
    bmp_destroy(image);
    return NULL;
  }

  buffer = malloc(sizeof(uint8_t) * bufferSize);
  if (buffer == NULL) {
    bmp_destroy(image);
    fclose(imageFile);
    return NULL;
  }

  readLength = fread(buffer, 1, bufferSize, imageFile);
  fclose(imageFile);
  if (readLength != bufferSize) {
    bmp_destroy(image);
    return NULL;
  }

  // Header
  _bmp_read_string(image->header.signature, 2, &buffer);
  _bmp_read_uint32(&image->header.filesize, &buffer);
  _bmp_read_uint32(NULL, &buffer);
  _bmp_read_uint32(&image->header.dataOffset, &buffer);

  // Info
  _bmp_read_uint32(&image->info.size, &buffer);
  _bmp_read_uint32(&image->info.width, &buffer);
  _bmp_read_uint32(&image->info.height, &buffer);
  _bmp_read_uint16(&image->info.planes, &buffer);
  _bmp_read_uint16(&image->info.bitsPerPixel, &buffer);
  _bmp_read_uint32(&image->info.compression, &buffer);
  _bmp_read_uint32(&image->info.imageSize, &buffer);
  _bmp_read_uint32(&image->info.xPixelsPerMeter, &buffer);
  _bmp_read_uint32(&image->info.yPixelsPerMeter, &buffer);
  _bmp_read_uint32(&image->info.colorsUsed, &buffer);
  _bmp_read_uint32(&image->info.numberOfImportantColors, &buffer);

  // Color
  if (image->info.bitsPerPixel < 8) {
    fprintf(stderr, "READING COLOR TABLE NOT SUPPORTED\n");
    exit(EXIT_FAILURE);
  }

  // Pixel Data
  image->numberOfPixels = image->info.width * image->info.height;
  image->pixel = calloc(image->numberOfPixels, sizeof(bmp_pixel_t));
  if (image->pixel == NULL) {
    bmp_destroy(image);
    return NULL;
  }

  for (unsigned long i = 0; i < image->numberOfPixels; ++i) {
    bmp_pixel_t *pixel = &image->pixel[i];
    _bmp_read_uint8(&pixel->blue, &buffer);
    _bmp_read_uint8(&pixel->green, &buffer);
    _bmp_read_uint8(&pixel->red, &buffer);
  }

  return image;
}

void bmp_destroy(bmp_image_t *image) {
  if (image == NULL) {
    return;
  }

  if (image->pixel != NULL) {
    free(image->pixel);
    image->pixel = NULL;
  }

  free(image);
  image = NULL;
}

int main(int argc, char **argv) {
  bmp_image_t *pic;
  const char *filepath = DEFAULT_FILE_PATH;
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
  bmp_pixel_t pixel;
  for (unsigned long i = 0; i < pic->numberOfPixels; ++i) {
    pixel = pic->pixel[i];
    printf("R%3u ", pixel.red);
    printf("G%3u ", pixel.green);
    printf("B%3u\t", pixel.blue);
    printf("#%02X%02X%02X", pixel.red, pixel.green, pixel.blue);
    printf("\t%lu\n", i);
  }

  bmp_destroy(pic);
  return 0;
}
