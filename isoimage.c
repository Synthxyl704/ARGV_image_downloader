#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "curl_utils.h"
#include "magickwand.h"

// #include "magickwand.c"

// !CHECK GITHUB README FOR ARGVTD FOR KNOWING THE ERROR/PRINT FORMAT USED! //

int imageUseCounter = 1;

char bufferForFileName[0x7F];
char buffer2ForFileNameVerification[0x7F];
char bufferForResizedFileName[0x7F];
char bufferForGSC_FileName[0x7F];
char bufferForRotatedFileName[0x7F];

// Default custom path - change this to your preferred directory
#define DEFAULT_CUSTOM_PATH "./Downloads"

// honestly i have no idea why i did this but maybe imma remove it l8r :z
typedef int (*printf_ptr)(const char *, ...);
printf_ptr vSTAT = printf;

// create path valid
int validateAndCreatePath(const char *path) {
    struct stat st = {0};
    
    if (stat(path, &st) == -1) {
        if (mkdir(path, 0755) == -1) {
          
          // 0755
          // 0 = octal literal
          // 7 = owner can rwx
          // 5 = group can rwx
          // 5 = other can rwx

            return EXIT_FAILURE;
        }
        vSTAT("std:directory_created::[%s]\n", path);
    } else if (!S_ISDIR(st.st_mode)) { // wow
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS; // Success
}

// concatenation support
void normalizePath(char *dest, const char *src, size_t destSize) {
    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
    
    size_t len = strlen(dest);
    if (len > 0 && dest[len - 1] != '/') {
        if (len < destSize - 1) {
            dest[len] = '/';
            dest[len + 1] = '\0';
        }
    }
}

int main(int argc, char **argv) {
  char siteImgURL[0x400];     // going to store the site URL here
  char *resolution = NULL;    // will store the resolution string from argv[2]
  char customPath[0x100] = {0}; // buffer for custom path
  char finalPath[0x120] = {0}; // buffer for final normalized path

  if (argc <= 1 || argv[1] == NULL || strlen(argv[1]) == 0) {
    fprintf(stderr, "%s", "std:CURL_initialization_failure:::CURL!=NULL\n");
    vSTAT("\nstd:download_failure::argv[1]=siteURL=NULL:::siteURL=valid\n");
    return EXIT_FAILURE;
  }

  // put B4 everything so that it can happen in that set dir instead
  if (argc > 6 && argv[6] != NULL && strlen(argv[6]) > 0) {

    // user provided custom path via argv[6]
    if (validateAndCreatePath(argv[6])) {
      normalizePath(finalPath, argv[6], sizeof(finalPath));
      vSTAT("std:custom_path_set::[%s]\n", finalPath);
    } else {
      fprintf(stderr, "std:invalid_custom_path::[%s]:::using_current_directory\n", argv[6]);
      strcpy(finalPath, "./");
    }
  } else {

    // -> use custom default path
    // -> if X -> return to current dir where prog is there
    // -> works

    if (validateAndCreatePath(DEFAULT_CUSTOM_PATH)) {
      normalizePath(finalPath, DEFAULT_CUSTOM_PATH, sizeof(finalPath));
      vSTAT("std:default_custom_path_used::[%s]\n", finalPath);
    } else {
      strcpy(finalPath, "./");
      vSTAT("std:using_current_directory::default_path_failed\n");
    }
  }

  // logic for not overrwriting the exisitng file, instead make another one while retaining previous ones
  // TODO: allow for independency later (is that even a word?)
  while (1 == 1) {
    snprintf(buffer2ForFileNameVerification, sizeof(buffer2ForFileNameVerification), "%siso[%d].jpg", finalPath, imageUseCounter);
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
  snprintf(bufferForFileName, sizeof(bufferForFileName), "%siso[%d]%s", finalPath, imageUseCounter, ext);
  const char *outputFilename = bufferForFileName;

  snprintf(bufferForGSC_FileName, sizeof(bufferForGSC_FileName), "%siso[%d]_GSCd%s", finalPath, imageUseCounter, ext);

  if (argc > 2) {
    snprintf(bufferForResizedFileName, sizeof(bufferForResizedFileName), "%siso[%d]_resized%s", finalPath, imageUseCounter, ext);
  }

   // argv[1] = URL
   // argv[2] = WIDTHxHEIGHT
   // argv[3] = GSC/GSC=NULL
   // argv[4] = extension(s)
   // argv[5] = rotate=[X]deg
   // argv[6] = custom (directory) path

  CURL *curl;
  FILE *filePointer;
  CURLcode response;

  strncpy(siteImgURL, argv[1], sizeof(siteImgURL) - 1);  // copy argv[1] (link) into siteImgURL buffer
  siteImgURL[sizeof(siteImgURL) - 1] = '\0';             // not risking it

  if (argc > 2 && argv[2] != NULL && strlen(argv[2]) > 0) {
    resolution = argv[2];                                // argv[2] = resolution in format "WIDTHxHEIGHT" only
    // printf("\n -> std:resolution_requested:::[%s] <-\n", resolution);
    vSTAT("\n-> std:resolution_requested:::[%s] <-\n\n", resolution);
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
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);                    // need to follow redirects to the argv[1] incase of disturbances

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
      vSTAT("std:ogimg_download_successful::[%s]\n", outputFilename);

      // what if res = specified?
      if (resolution != NULL) {
        vSTAT("\nstd:processing_resize::resolution_appliedValueParameters:[%s]\n", resolution);  // as a string LOL :skull:
        
        if (resizeImage(outputFilename, bufferForResizedFileName, resolution) == EXIT_SUCCESS) {
          // remove(outputFilename);
          // printf("std:original_replaced:::kept only resized version\n");
        } else {
          fprintf(stderr, "std:resize_failed::resolution=[%s]\n", resolution);
        }
      }
    }

    curl_easy_cleanup(curl);     // basically free();
    // printf("\nstd:http_code::[%ld]\n", http_code); // 200 = successful request
    // refer: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes

  } else {
    fprintf(stderr, "%s", "std:CURL_initialization_failure:::CURL!=NULL\n");
    return EXIT_FAILURE;
  }

  // =============================================================================
  // GRAYSCALING!!!
  // Handle grayscale conversion after successful download

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
      fprintf(stderr, "std:GSC_ARGV[%d]:::set_GSC_status=valid\n",  0 + 3);
    }
  } 

  // ==================1===============================
  // EXTENSION HANDLING

  if (argc > 4) {
    const char *validExtension[] = {"jpg", "jpeg", "png", "svg", "tiff", "webp", "heif", "heic", "xcf", "exif"};
    const char *userInputtedExtensionRaw = argv[4]; 

    if (userInputtedExtensionRaw[0] == '.') {
      userInputtedExtensionRaw += 1;
    }

    int isValid = 0;

    for (int inc = 0; inc < sizeof(validExtension) / sizeof(validExtension[0]); ++inc) {
      if (strcasecmp(userInputtedExtensionRaw, validExtension[inc]) == 0) {
        isValid += 1;
        break;
      }
    }

    if (isValid == 1) {
      const char *originalFile = outputFilename; // Use outputFilename instead of argv[1]     

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
      const char *dot = strrchr(fileNameOnly, '.');                                  // replace only the text [>last_dot] 
      size_t baseLength = dot ? (size_t)(dot - fileNameOnly) : strlen(fileNameOnly); // pointer arithmetic!!!1!

      char modifiedFileName[0x7F];
      snprintf(modifiedFileName, sizeof(modifiedFileName), "%siso[%d]_modifiedEXT.%s", finalPath, imageUseCounter, userInputtedExtensionRaw);

      vSTAT("\nstd:user_extension_override::[%s] => [%s]\n\n", fileNameOnly, modifiedFileName);

      // UNCOMMENT THIS IF YOU WANT TO OVERWRITE THE ORIGINAL SAVED IMAGE!
      // snprintf(bufferForFileName, sizeof(bufferForFileName), "%s", modifiedFileName);
    }

    else {
      fprintf(stderr, "std:invalid_extension_requested::[%s]\n", argv[4]);
    }
  } 

if (argc > 5 && argv[5] != NULL) {
    snprintf(bufferForRotatedFileName, sizeof(bufferForRotatedFileName), 
             "%siso[%d]_rotated%s", finalPath, imageUseCounter, ext);
    
    const char *inputForRotation = outputFilename;
    if (resolution != NULL && access(bufferForResizedFileName, F_OK) == 0) {
        inputForRotation = bufferForResizedFileName;
    } 
    if (argc > 3 && strcasecmp(argv[3], "GSC") == 0 && access(bufferForGSC_FileName, F_OK) == 0) {
        inputForRotation = bufferForGSC_FileName;
    }
    
    // "rotate=[X]deg"
    char rotationArg[32] = {0};
    if (strcasecmp(argv[5], "rotate=NULL") == 0) {
      vSTAT("\nstd::rotation_conduction_SKIPPED\n\n");
      return 0;
    }

    if (strncmp(argv[5], "rotate=", 7) == 0) {
      snprintf(rotationArg, sizeof(rotationArg), "rotate(%s", argv[5]+7);
    } else {
      strncpy(rotationArg, argv[5], sizeof(rotationArg) - 1);
    }
    
    if (rotateImage(inputForRotation, bufferForRotatedFileName, rotationArg) != EXIT_SUCCESS) {
        fprintf(stderr, "std:rotation_failed::[%s]\n", argv[5]);
    }
}

  // if (argc > <change_argc_here>) {
  //   if (strcasecmp(argv[5], "ls_argv") == 0) {
  //     vSTAT("\n\n");

  //     for (int inc = 0; inc <= argc; inc += 1) {
  //       vSTAT("std:argv[%d]: [%s]\n", inc, argv[inc]);
  //     }

  //     vSTAT("\n\n");
  //   }
  // }

  return EXIT_SUCCESS;
}
