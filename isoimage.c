#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "curl_utils.h"
#include "magickwand.h"

int imageUseCounter = 1;
char bufferForFileName[0x7F];
char buffer2ForFileNameVerification[0x7F];
char bufferForResizedFileName[0x7F];

int main(int argc, char **argv) {
  char siteImgURL[0x400];     // going to store the site URL here
  char *resolution = NULL;    // will store the resolution string from argv[2]

  // logic for not overrwriting the exisitng file, instead make another one while retaining previous ones
  while (1) {
    snprintf(buffer2ForFileNameVerification, sizeof(buffer2ForFileNameVerification), "./iso[%d].jpg", imageUseCounter);
    if (access(buffer2ForFileNameVerification, F_OK) != 0) {
      break;
    } else {
      imageUseCounter += 1;
    }
  }

  // unused
  const char *ext = strrchr(argv[1] ? argv[1] : "", '.');
  if (!ext || !(strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0 || strcmp(ext, ".png") == 0)) {
    ext = ".jpg";
  }

  // set the output filename (retain original buffer for actual writing for no reason)
  snprintf(bufferForFileName, sizeof(bufferForFileName), "./iso[%d]%s", imageUseCounter, ext);
  const char *outputFilename = bufferForFileName;

  if (argc > 2) {
    snprintf(bufferForResizedFileName, sizeof(bufferForResizedFileName), "./iso[%d]_resized%s", imageUseCounter, ext);
  }

  CURL *curl;
  FILE *filePointer;
  CURLcode response;

  // main argument parsing
  if (argc > 1) {
    strncpy(siteImgURL, argv[1], sizeof(siteImgURL) - 1);  // copy argv[1] (link) into siteImgURL buffer
    siteImgURL[sizeof(siteImgURL) - 1] = '\0';              

    if (argc > 2) {
      resolution = argv[2];  // argv[2] = resolution in format "WIDTHxHEIGHT"
      printf("\n -> std:resolution_requested:::[%s] <-\n", resolution);
    }
  } else {
    fprintf(stderr, "%s", "std:file_inclusion_error:::include_argv[1]=true\n");  
    // follows my compiler error format | [:] program standard | [:::] amelioration tweak string
    // fprintf(stderr, "std:usage=%s_<image_url>_[optional]-reisize_res_ints\n", argv[0]);         
    // github repo will have the tutorial thingy
    return EXIT_FAILURE;
  }

  curl = curl_easy_init();
  if (curl != NULL) {
    filePointer = fopen(outputFilename, "wb"); // fopen(file, readtype); here wb = web binary format
    if (!filePointer) {
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
    } else if (http_code != 200) {
      fprintf(stderr, "std:http_error::http_code_[%ld]\n", http_code);
      curl_easy_cleanup(curl);
      return EXIT_FAILURE;
    } else {
      // FILE *testFile = fopen(outputFilename, "rb");
      printf("\nstd:download_successful::[%s]\n", outputFilename);

      // what if res = specified?
      if (resolution != NULL) {
        printf("std:processing_resize::resolution_appliedValueParameters:[%s]\n", resolution);  // as a string LOL :skull:
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
