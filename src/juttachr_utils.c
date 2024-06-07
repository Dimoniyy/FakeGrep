#include "juttachr_utils.h"

// target_ptr is a pointer to a string.
void strRip(char** target_ptr, const size_t from, const size_t to) {
  size_t i = 0;
  char* buffer = strduplicate(*target_ptr);
  if (to <= strlen(buffer)) {
    buffer[to] = '\0';
  }
  if (from <= strlen(buffer)) {
    strcpy(*target_ptr, buffer + from);
  } else if (from <= to) {
    for (i = 0; i <= sizeof(*target_ptr) * sizeof(char); i++) {
      (*target_ptr)[i] = '\0';
    }
  }
  if (buffer != NULL) {
    free(buffer);
  }
}

// allocates memory!
char* strduplicate(const char* buffer) {
  char* output = NULL;
  output = malloc((strlen(buffer) + 1) * sizeof(char));
  if (output != NULL) {
    memcpy(output, buffer, (strlen(buffer) + 1) * sizeof(char));
  }
  return output;
}

// allocates memory at destination!
long long getLineAndAlloc(char** destination, size_t* size_of_destination,
                          FILE* stream) {
  char* destination_new_ptr;
  size_t i = 0;
  char c = 'k';
  if (stream == NULL) {
    c = '\n';
    i = 1;
  }
  if (destination != NULL && size_of_destination != NULL) {
    if (*destination == NULL) {
      (*destination) = malloc(sizeof(char) * 8);
      *size_of_destination = (sizeof(char) * 8);
    }
  }
  for (; c != '\n' && c != EOF && destination != NULL &&
         size_of_destination != NULL;
       i++) {
    if (i < 12000) {
      c = fgetc(stream);
      destination_new_ptr = realloc(*destination, sizeof(char) * i + 2);
      if (destination_new_ptr != NULL) {
        *destination = destination_new_ptr;
        (*destination)[i] = (c == EOF ? '\n' : c);
      } else {
        printf("allocation error");
        c = EOF;
      }
    } else {
      c = '\n';
    }
  }
  (*destination)[i] = '\0';
  if (c == EOF && i == 1) {
    i = -1;
  }
  (*destination)[i + 1] = '\0';
  return i;
}
