#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// [S1] - write site linked data to a local file
size_t writeDataToLocalFile(void *ptr, size_t size, size_t nmemb, FILE *stream) { // nmemb = number of elements to write to the file 
  return fwrite(ptr, size, nmemb, stream);                                        //   ptr = ptr to data to write
                                                                                  //  size = size of each data element
                                                                                  //stream = opens ptr to a file where i have to save the data to
}

int imageUseCounter = 1;
char bufferForFileName[0x7F];

int main(int argc, char **argv) {
  char siteImgURL[0x400];     // going to store the site URL here
  sprintf(bufferForFileName, "iso[%d].jpg", imageUseCounter);
  const char *outputFilename = bufferForFileName;

  CURL *curl;
  FILE *filePointer;          // destination file pointer here
  CURLcode response;          // result status of a CURL op and also to start executing the func later on

  // gonna use cmd line because way cooler
  if (argc > 1) {
    strncpy(siteImgURL, argv[1], sizeof(siteImgURL) - 0x1);  // copy argv[1] (link) into siteImgURL buffer
    siteImgURL[sizeof(siteImgURL) - 1] = '\0';              
  } else {
    fprintf(stderr, "%s", "std:file_inclusion_error:::include_argv[1]=true\n");  // follows my compiler error format | [:] program standard | [:::] amelioration tweak string
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
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, filePointer);                // stores the data to my local filePointer i put
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);                    // need to follow redirects to the argv[1]?

    response = curl_easy_perform(curl); // does everything

    if (response != CURLE_OK) {
      fprintf(stderr, "std:download_failure::[%s]\n", curl_easy_strerror(response));
      //exit(EXIT_FAILURE);
    } else {
      printf("std:download_successful::[%s]\n", outputFilename);
    }

    fclose(filePointer);        
    curl_easy_cleanup(curl);     // basically free();
  } else {
    fprintf(stderr, "%s", "std:CURL_initialization_failure:::CURL!=NULL\n");
    return EXIT_SUCCESS; // here exit success because either i have to reinstate argv[1] or exit the program anyway for an actual link to CURL
  }

  return EXIT_SUCCESS;
}
