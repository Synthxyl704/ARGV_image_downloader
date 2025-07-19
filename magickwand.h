#ifndef MAGICKWAND_UTILS_H
#define MAGICKWAND_UTILS_H

#include <MagickWand/MagickWand.h>

// resizing + grayscaling | MACX
int resizeImage(const char *inputFile, const char *outputFile, const char *resolution);
int grayscaleImage(const char *inputPath, const char *outputPath);

int rotateImage(const char *inputFile, const char *outputFile, const char *rotationCommand);

#endif
