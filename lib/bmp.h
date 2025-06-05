#ifndef BMP_H

#define BMP_H

#include <stdint.h>

#define BUFFER_SIZE 250

enum {
  BMP_READ,
  BMP_WRITE,
};

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
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
} bmp_pixel_t;

typedef struct {
  bmp_header_t header;
  bmp_info_t info;
  unsigned long numberOfPixels;
  bmp_pixel_t *pixel;
} bmp_image_t;

/**
 * @brief Load a bmp image from a file.
 *
 * Loads a ".bmp" image from a file into a `bmp_image_t` structure.
 *
 * @param filepath The path to the file that contains the picure.
 * @return A `bmp_image_t` pointer or `NULL` on error.
 */
bmp_image_t *bmp_load(const char *filepath);

/**
 * @brief Save a `bmp_image_t` to a file.
 *
 * Saves a bmp_image_t to a file, encoded as a ".bmp" file.
 *
 * @param image The image to save to the file.
 * @param filepath The location of where to save the file to.
 *
 * @return non-zero on error and `0` on success.
 */
int bmp_save_image(bmp_image_t *image, char *filepath);

/**
 * @brief Destorys a loaded bmp image
 *
 * Frees all memory related to a `bmp_image_t`.
 *
 * @param image An image that was loaded with `bmp_load`.
 */
void bmp_destroy(bmp_image_t *image);

#endif // BMP_H
