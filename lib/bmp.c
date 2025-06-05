#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

int _bmp_read_string(uint8_t *dest, unsigned int n, uint8_t **buffer) {
  memmove(dest, *buffer, n);
  *buffer += n;
  return n;
}

int _bmp_read(void *dest, uint8_t **buffer, unsigned int n) {
  long i = 0;
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

int _bmp_write_string(unsigned char *string, unsigned long n, FILE *imageFile) {
  fwrite(string, n, 1, imageFile);
  return n;
}

int _bmp_write(uint64_t *src, FILE *imageFile, unsigned long n) {
  long i = 0;
  unsigned long writeNumber = 0;

  for (i = 0; i < n; ++i) {
    if (src == NULL) {
      fwrite("\0", 1, 1, imageFile);
    } else {
      uint8_t writeValue = (*src >> (i * 8)) & 0xFF;
      fwrite(&writeValue, 1, 1, imageFile);
    }
  }

  return n;
}

int _bmp_write_uint8(uint8_t *src, FILE *imageFile) {
  return _bmp_write((uint64_t *)src, imageFile, 1);
}

int _bmp_write_uint16(uint16_t *src, FILE *imageFile) {
  return _bmp_write((uint64_t *)src, imageFile, 2);
}

int _bmp_write_uint32(uint32_t *src, FILE *imageFile) {
  return _bmp_write((uint64_t *)src, imageFile, 4);
}

FILE *_bmp_open_file(const char *filepath, unsigned long *fileSize,
                     int openMethod) {
  FILE *imageFile;
  unsigned long length = 0;

  switch (openMethod) {
  case BMP_READ:
    imageFile = fopen(filepath, "rb");
    break;
  case BMP_WRITE:
    imageFile = fopen(filepath, "wb");
    break;
  default:
    imageFile = NULL;
  }
  if (!imageFile) {
    return NULL;
  }

  if (fseek(imageFile, 0, SEEK_END) == -1) {
    fclose(imageFile);
    return NULL;
  }

  length = ftell(imageFile);
  if (length == -1) {
    fclose(imageFile);
    return NULL;
  }

  rewind(imageFile);

  if (fileSize != NULL) {
    *fileSize = length;
  }

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

  imageFile = _bmp_open_file(filepath, &bufferSize, BMP_READ);
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

    if (image->info.bitsPerPixel == 32) {
      _bmp_read_uint8(&pixel->alpha, &buffer);
    } else {
      pixel->alpha = 0xFF;
    }
  }

  return image;
}

int bmp_save_image(bmp_image_t *image, char *filepath) {
  FILE *imageFile;
  imageFile = _bmp_open_file(filepath, NULL, BMP_WRITE);
  if (!imageFile) {
    return -1;
  }

  // Header
  _bmp_write_string(image->header.signature, 2, imageFile);
  _bmp_write_uint32(&image->header.filesize, imageFile);
  _bmp_write_uint32(NULL, imageFile);
  _bmp_write_uint32(&image->header.dataOffset, imageFile);

  // Info
  _bmp_write_uint32(&image->info.size, imageFile);
  _bmp_write_uint32(&image->info.width, imageFile);
  _bmp_write_uint32(&image->info.height, imageFile);
  _bmp_write_uint16(&image->info.planes, imageFile);
  _bmp_write_uint16(&image->info.bitsPerPixel, imageFile);
  _bmp_write_uint32(&image->info.compression, imageFile);
  _bmp_write_uint32(&image->info.imageSize, imageFile);
  _bmp_write_uint32(&image->info.xPixelsPerMeter, imageFile);
  _bmp_write_uint32(&image->info.yPixelsPerMeter, imageFile);
  _bmp_write_uint32(&image->info.colorsUsed, imageFile);
  _bmp_write_uint32(&image->info.numberOfImportantColors, imageFile);

  // Color
  if (image->info.bitsPerPixel < 8) {
    fprintf(stderr, "WRITING COLOR TABLE NOT SUPPORTED\n");
    exit(EXIT_FAILURE);
  }

  // Pixel Data
  for (unsigned long i = 0; i < image->numberOfPixels; ++i) {
    bmp_pixel_t *pixel = &image->pixel[i];

    _bmp_write_uint8(&pixel->blue, imageFile);
    _bmp_write_uint8(&pixel->green, imageFile);
    _bmp_write_uint8(&pixel->red, imageFile);

    if (image->info.bitsPerPixel == 32) {
      _bmp_write_uint8(&pixel->alpha, imageFile);
    }
  }

  fclose(imageFile);

  return 0;
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
