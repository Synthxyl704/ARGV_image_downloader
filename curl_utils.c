#include "curl_utils.h"

size_t writeDataToLocalFile(void *ptr, size_t size, size_t nmemb, FILE *stream) { //  nmemb = number of elements to write to the file 
  return fwrite(ptr, size, nmemb, stream);                                        //    ptr = ptr to data to write
                                                                                  //   size = size of each data element
                                                                                  // stream = opens ptr to a file where i have to save the data to
}
