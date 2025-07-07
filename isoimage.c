#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "curl_utils.h"
#include "magickwand.h"

// !CHECK GITHUB README FOR ARGVTD FOR KNOWING THE ERROR/PRINT FORMAT USED! //

int imageUseCounter = 1;

char bufferForFileName[0x7F];
char buffer2ForFileNameVerification[0x7F];
char bufferForResizedFileName[0x7F];
char bufferForGSC_FileName[0x7F];

typedef int (*printf_ptr)(const char *, ...);
printf_ptr vSTAT = printf;

int main(int argc, char **argv) {
  char siteImgURL[0x400];     // going to store the site URL here
  char *resolution = NULL;    // will store the resolution string from argv[2]

  // logic for not overrwriting the exisitng file, instead make another one while retaining previous ones
  while (1 == 1) {
    snprintf(buffer2ForFileNameVerification, sizeof(buffer2ForFileNameVerification), "./iso[%d].jpg", imageUseCounter);
    if (access(buffer2ForFileNameVerification, F_OK) != 0) {
      break;
    } else {
      imageUseCounter += 1;
    }
  }

  // dont ask me what i was on while writing this
  const char *ext = strrchr(argv[1] ? argv[1] : "", '.');
  if (!ext || !(strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0 || strcmp(ext, ".png") == 0)) {
    ext = ".jpg"; // png later?
  }

  // set the output filename (retain original buffer for actual writing for no reason)
  snprintf(bufferForFileName, sizeof(bufferForFileName), "./iso[%d]%s", imageUseCounter, ext);
  const char *outputFilename = bufferForFileName;

  snprintf(bufferForGSC_FileName, sizeof(bufferForGSC_FileName), "./iso[%d]_GSCd%s", imageUseCounter, ext);

  if (argc > 2) {
    snprintf(bufferForResizedFileName, sizeof(bufferForResizedFileName), "./iso[%d]_resized%s", imageUseCounter, ext);
  }

  CURL *curl;
  FILE *filePointer;
  CURLcode response;

  if (argc > 1) {
    strncpy(siteImgURL, argv[1], sizeof(siteImgURL) - 1);  // copy argv[1] (link) into siteImgURL buffer
    siteImgURL[sizeof(siteImgURL) - 1] = '\0';             // not risking it

    if (argc > 2) {
      resolution = argv[2];                                // argv[2] = resolution in format "WIDTHxHEIGHT" only
      // printf("\n -> std:resolution_requested:::[%s] <-\n", resolution);
      vSTAT("-> std:resolution_requested:::[%s] <-\n", resolution);
    }

    if (argc > 3) {
      if (strcasecmp(argv[3], "GSC") == 0) {
        const char *inputForGSC = (resolution != NULL) ? bufferForResizedFileName : outputFilename;
        if (grayscaleImage(inputForGSC, bufferForGSC_FileName) == EXIT_SUCCESS) {
          // printf("std:grayscale_successful::[%s]\n", bufferForGSC_FileName);
          vSTAT("std:grayscale_successful::[%s]\n", bufferForGSC_FileName);        
        } 
      } else if (strcasecmp(argv[3], "GSC=NULL") == 0) {
        vSTAT("\nstd:grayscale_function::user_status_set=NULL\n");
      } else {
        fprintf(stderr, "std:GSC_ARGV[%d]_parse_error:::set_GSC_status=valid",  0 + 3);
      }
    } 

    if (argc > 4) {
    const char *validExtension[] = {"jpg", "jpeg", "png", "svg", "tiff", "webp", "heif", "heic", "xcf", "exif"};
    const char *userInputtedExtensionRaw = argv[4]; 

    if (userInputtedExtensionRaw[0] == '.') {
      userInputtedExtensionRaw += 1;
    }

    int isValid = false;

    for (int inc = 0; inc < sizeof(validExtension) / sizeof(validExtension[0]); ++inc) {
      if (strcasecmp(userInputtedExtensionRaw, validExtension[inc]) == 0) {
        isValid = true;
        break;
      }
    }

    if (isValid == true) {
      const char *originalFile = argv[1];       

      // this may get confusing
      // basically at compile/runtime (idk which ones the correct term), argv[1] has already transmutated into a "FILE* [x] = "~/<path>";
      // char *originalFile = argv[1]; saves the original image path to itself
      // so it is essentially pointing to the path again which is already saved into your directory of storage
      // that is why you dont directly mess with siteImgURL because it has already been converted to a path by FILE*
      // if messed with, it will cause alot of breakings since URLs dont usually contains slashes 
      // and will cause conditional extension invalidation

      const char *lastSlash = strrchr(originalFile, '/');                            // check for the LAST [/] to occur
      const char *fileNameOnly = ((lastSlash) ? lastSlash + 1 : originalFile);       // want to isolate the fileName only? lastSlash + 1 = filename!

      // index:       0   1   2   3   4   5   6
      // character:   s   e   x   .   p   n   g
      // pointerLoc:  ↑           ↑
      //        fileNameOnly    dot (at index 3)
      //        (points here!) because ~/path/anotherPath/sex.png, we only want it to count until last slash ie until (s)  

      // current [.<user_file_extens^n] -> [.<argv[4]>] conversion
      const char *dot = strrchr(fileNameOnly, '.');                                  // replace only the text >last_dot 
      size_t baseLength = dot ? (size_t)(dot - fileNameOnly) : strlen(fileNameOnly); // pointer arithmetic!!!1!

      char modifiedFileName[0x100];
      snprintf(modifiedFileName, sizeof(modifiedFileName), "./iso[%d]_modifiedEXT.%s", imageUseCounter, userInputtedExtensionRaw);

      vSTAT("std:user_extension_override::[%s] => [%s]\n", fileNameOnly, modifiedFileName);

      // UNCOMMENT THIS IF YOU WANT TO OVERWRITE THE ORIGINAL SAVED IMAGE!
      // snprintf(bufferForFileName, sizeof(bufferForFileName), "%s", modifiedFileName);
    }

    else {
      fprintf(stderr, "std:invalid_extension_requested::[%s]\n", argv[4]);
    }
  }

  if (argc > 5) {
    if (strcasecmp(argv[5], "ls_argv") == 0) {
      vSTAT("\n\n");

      for (int inc = 0; inc <= argc; inc += 1) {
        vSTAT("std:argv[%d]: [%s]\n", inc, argv[inc]);
      }

      vSTAT("\n\n");
    }
  }
  
  else {
    fprintf(stderr, "%s", "std:file_inclusion_error:::include_argv[1]=true\n"); 
    return EXIT_FAILURE;
  }
} 

  else if (argv[1] == NULL || !argv[1]) {
    vSTAT("std:download_failure::argv[1]=siteURL=NULL:::siteURL=valid");
    return EXIT_FAILURE;
  }

  curl = curl_easy_init();
  if (curl != NULL) {
    filePointer = fopen(outputFilename, "wb");                             // fopen(file, readtype); here wb = web binary format
    if (!filePointer || filePointer == NULL) {
      // perror("std:fatal_file_error:::fileinclude==true");
      fprintf(stderr, "%s", "std:fatal_file_read_error:::fileToRead!=NULL\n");
      curl_easy_cleanup(curl);
      return EXIT_FAILURE;
    }

    curl_easy_setopt(curl, CURLOPT_URL, siteImgURL);                       // basically goes to argv[1] site
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeDataToLocalFile);   // saves the data from argv[1] to wDTLF 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, filePointer);                // stores the data to the local filePointer i put
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);                    // need to follow redirects to the argv[1]?

    response = curl_easy_perform(curl); // does everything

    fclose(filePointer); // <-- make sure file is closed BEFORE reading or processing

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (response != CURLE_OK) {
      fprintf(stderr, "std:download_failure::[%s]\n", curl_easy_strerror(response));
      curl_easy_cleanup(curl);
      return EXIT_FAILURE;
    } 
    
    else if (http_code != 200) {
      fprintf(stderr, "std:http_error::http_code=[%ld]\n", http_code);
      curl_easy_cleanup(curl);
      return EXIT_FAILURE;
    } 
    
    else {
      // FILE *testFile = fopen(outputFilename, "rb");  // read binary
      vSTAT("std:download_successful::[%s]\n", outputFilename);

      // what if res = specified?
      if (resolution != NULL) {
        vSTAT("std:processing_resize::resolution_appliedValueParameters:[%s]\n", resolution);  // as a string LOL :skull:
        
        if (resizeImage(outputFilename, bufferForResizedFileName, resolution) == EXIT_SUCCESS) {
          // remove(outputFilename);
          // printf("std:original_replaced:::kept only resized version\n");
        }
      }
    }

    curl_easy_cleanup(curl);     // basically free();
    // printf("\nstd:http_code::[%ld]\n", http_code); // 200 = successful request

  } else {
    fprintf(stderr, "%s", "std:CURL_initialization_failure:::CURL!=NULL\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
