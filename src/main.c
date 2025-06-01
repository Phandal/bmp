/* #include <stdint.h> */
#include <stdio.h>

struct bmp_header {
  unsigned char signature[2];
  unsigned long filesize;
  unsigned long dataOffset;
};

struct bmp_info_header {
  unsigned long size;
  unsigned long width;
  unsigned long height;
  unsigned long planes;
  unsigned int  bitsPerPixel;
  unsigned int compression;
};
  

struct bmp {
  char *filepath;
  struct bmp_header header;
  struct bmp_info_header info;
};

struct bmp pic = {
  .filepath = "res/wall.bmp",
  .header = {0},
  .info = {0},
};

int main(void) {
  unsigned char buffer[100];
  FILE *file = fopen(pic.filepath, "rb");
  if (file == NULL) {
    perror("could not open file:");
    return -1;
  }

  fread(pic.header.signature, 1, 2, file); // Signature
  fread(buffer, 1, 4, file); // File Size 
  for (int i = 3; i >= 0; --i) {
    pic.header.filesize = (pic.header.filesize << 8) | buffer[i];
  }

  fread(buffer, 1, 4, file); // Reserved
  fread(buffer, 1, 4, file); // DataOffset
  for (int i = 3; i >= 0; --i) {
    pic.header.dataOffset = (pic.header.dataOffset << 8) | buffer[i];
  }

  fread(buffer, 1, 4, file); // Size
  for (int i = 3; i >= 0; --i) {
    pic.info.size = (pic.info.size << 8) | buffer[i];
  }
  fread(buffer, 1, 4, file); // Width
  for (int i = 3; i >= 0; --i) {
    pic.info.width = (pic.info.width << 8) | buffer[i];
  }
  fread(buffer, 1, 4, file); // Height
  for (int i = 3; i >= 0; --i) {
    pic.info.height = (pic.info.height << 8) | buffer[i];
  }
  fread(buffer, 1, 2, file); // Planes
  for (int i = 1; i >= 0; --i) {
    pic.info.planes = (pic.info.planes << 8) | buffer[i];
  }
  fread(buffer, 1, 2, file); // BitsPerPixel
  for (int i = 1; i >= 0; --i) {
    pic.info.bitsPerPixel = (pic.info.bitsPerPixel << 8) | buffer[i];
  }
  fread(buffer, 1, 4, file); // Compression
  for (int i = 3; i >= 0; --i) {
    pic.info.compression = (pic.info.compression << 8) | buffer[i];
  }

  printf("pic.header.signature: %s\n", pic.header.signature);
  printf("pic.header.filesize: %lu\n", pic.header.filesize);
  printf("pic.header.dataOffset: %lu\n", pic.header.dataOffset);
  printf("pic.info.size: %lu\n", pic.info.size);
  printf("pic.info.width: %lu\n", pic.info.width);
  printf("pic.info.height: %lu\n", pic.info.height);
  printf("pic.info.planes: %lu\n", pic.info.planes);
  printf("pic.info.bitsPerPixel: %d\n", pic.info.bitsPerPixel);
  printf("pic.info.compression: %d\n", pic.info.compression);
  printf("Position in file: 0x%02lX\n", ftell(file));

  fclose(file);
  return 0;
}
