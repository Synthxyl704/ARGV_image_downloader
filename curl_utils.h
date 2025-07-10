#ifndef CURL_UTILS_H
#define CURL_UTILS_H

#include <stdio.h>
#include <curl/curl.h>

size_t writeDataToLocalFile(void *ptr, size_t size, size_t nmemb, FILE *stream); //  nmemb = number of elements to write to the file 

#endif 
