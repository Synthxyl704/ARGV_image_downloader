#ifndef MAGICKWAND_UTILS_H
#define MAGICKWAND_UTILS_H

#include <MagickWand/MagickWand.h>

int resizeImage(const char *inputFile, const char *outputFile, const char *resolution);
int grayscaleImage(const char *inputPath, const char *outputPath);

#endif
