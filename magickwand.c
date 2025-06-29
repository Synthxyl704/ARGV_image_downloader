#include <stdio.h>
#include <stdlib.h>
#include <MagickWand/MagickWand.h>
#include "magickwand.h"

int resizeImage(const char *inputFile, const char *outputFile, const char *resolution) {
  MagickWand *magick_wand;
  MagickBooleanType status; 
  int width, height;
  
  if (sscanf(resolution, "%dx%d", &width, &height) != 2) {
    fprintf(stderr, "std:resolution_parse_error:::format=WIDTHxHEIGHT\n");
    return EXIT_FAILURE;
  }

  MagickWandGenesis(); // initialize ONLY ONCE !
  magick_wand = NewMagickWand();

  // og img
  status = MagickReadImage(magick_wand, inputFile);
  if (status == MagickFalse) {
     char *descStr;
     ExceptionType severity;
     descStr = MagickGetException(magick_wand, &severity);
     fprintf(stderr, "std:image_read_error::fileread_failure::[%s]\n", descStr);
     descStr = (char*) MagickRelinquishMemory(descStr);
     DestroyMagickWand(magick_wand);
     MagickWandTerminus();
     return EXIT_FAILURE;
  }

  // img resize func();
  status = MagickAdaptiveResizeImage(magick_wand, width, height);
  if (status == MagickFalse) {
    fprintf(stderr, "std:image_resize_error::failure_to_resize_at_[%dx%d]\n", width, height);
    DestroyMagickWand(magick_wand);
    MagickWandTerminus();
    return EXIT_FAILURE;
  }

  // write the resized image
  status = MagickWriteImage(magick_wand, outputFile);
  if (status == MagickFalse) {
    fprintf(stderr, "std:image_write_error:::[L55ERR]\n");
    DestroyMagickWand(magick_wand);
    MagickWandTerminus();
    return EXIT_FAILURE;
  }

  // free again
  DestroyMagickWand(magick_wand);
  MagickWandTerminus();

  printf("std:resize_successful::resized_to_[%dx%d]_filename=[%s]\n", width, height, outputFile);
  return EXIT_SUCCESS;
}

int grayscaleImage(const char *inputPath, const char *outputPath) {
    MagickWand *wand = NULL;
    MagickBooleanType status;

    MagickWandGenesis();
    wand = NewMagickWand();

    status = MagickReadImage(wand, inputPath);
    if (status == MagickFalse) {
        char *descStr;
        ExceptionType severity;
        descStr = MagickGetException(wand, &severity);
        fprintf(stderr, "std:image_read_error::fileread_failure::[%s]\n", descStr);
        descStr = (char*) MagickRelinquishMemory(descStr);
        wand = DestroyMagickWand(wand);
        MagickWandTerminus();
        return EXIT_FAILURE;
    }

    // Convert to grayscale
    status = MagickSetImageType(wand, GrayscaleType);
    if (status == MagickFalse) {
        fprintf(stderr, "std:grayscale_error::failed_to_convert\n");
        wand = DestroyMagickWand(wand);
        MagickWandTerminus();
        return EXIT_FAILURE;
    }

    status = MagickWriteImage(wand, outputPath);
    if (status == MagickFalse) {
        fprintf(stderr, "std:image_write_error:::[GRAYERR]\n");
        wand = DestroyMagickWand(wand);
        MagickWandTerminus();
        return EXIT_FAILURE;
    }

    wand = DestroyMagickWand(wand);
    MagickWandTerminus();
    printf("std:grayscale_successful::[%s]\n", outputPath);
    return EXIT_SUCCESS;
}
