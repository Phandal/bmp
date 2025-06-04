#include "bmp.h"

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

FILE *_bmp_open_file(const char *filepath, unsigned long *fileSize) {
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

  imageFile = _bmp_open_file(filepath, &bufferSize);
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
