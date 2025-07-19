#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  printf("std:resize_successful::resized_to_[%dx%d]_filename=[%s]\n\n", width, height, outputFile);

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

    // converts to grayscale
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

void CatchWandException(MagickWand *wand) {
    ExceptionType severity;
    char *description;

    description = MagickGetException(wand, &severity);
    fprintf(stderr, "std:magick_exception:::severity=%d:::description=%s\n", severity, description);
    MagickRelinquishMemory(description);
}

int rotateImage(const char *inputFile, const char *outputFile, const char *rotationCommand) {
    MagickWand *magick_wand = NULL;
    PixelWand *background = NULL;
    MagickBooleanType status;
    double degrees = 0.0;

    // Parse rotation command (accepts both "rotate(90deg)" and "rotate=90deg")
    if (sscanf(rotationCommand, "rotate(%lfdeg)", &degrees) != 1 &&
        sscanf(rotationCommand, "rotate=%lfdeg", &degrees) != 1) {
        fprintf(stderr, "std:rotation_parse_error:::format=rotate(90deg)_or_rotate=90deg\n");
        return EXIT_FAILURE;
    }

    MagickWandGenesis();
    magick_wand = NewMagickWand();
    if (magick_wand == NULL) {
        fprintf(stderr, "std:magickwand_creation_failure\n");
        MagickWandTerminus();
        return EXIT_FAILURE;
    }

    // Read input image
    status = MagickReadImage(magick_wand, inputFile);
    if (status == MagickFalse) {
        CatchWandException(magick_wand);
        goto cleanup;
    }

    // Configure rotation background
    background = NewPixelWand();
    if (background == NULL) {
        fprintf(stderr, "std:pixelwand_creation_failure\n");
        goto cleanup;
    }
    PixelSetColor(background, "transparent");
    MagickSetImageBackgroundColor(magick_wand, background);

    // Rotate the image
    status = MagickRotateImage(magick_wand, background, degrees);
    if (status == MagickFalse) {
        CatchWandException(magick_wand);
        goto cleanup;
    }

    // Write output
    status = MagickWriteImage(magick_wand, outputFile);
    if (status == MagickFalse) {
        CatchWandException(magick_wand);
        goto cleanup;
    }

    printf("std:rotation_successful::[%.1f_degrees]::[%s]\n\n", degrees, outputFile);

cleanup:
    if (background) DestroyPixelWand(background);
    if (magick_wand) DestroyMagickWand(magick_wand);
    MagickWandTerminus();
    return (status == MagickFalse) ? EXIT_FAILURE : EXIT_SUCCESS;
}
