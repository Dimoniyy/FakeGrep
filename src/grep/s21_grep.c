#include "./s21_grep.h"

int main(int argc, char* argv[]) {
  int arguments = 0x0, q_i = 0, i_files_to_process = 0, use_as_query_n = 0,
      has_query_as_e_f_option = 0, t = 0;

  char* buffer = NULL;
  char* files_names[1024];
  char* temppath_query;
  regex_t reegex;
  FILE* query_file = NULL;
  FILE* stream;

  for (int i = (argc - 1); i > 0; i--) {
    if (argv[i][0] != '-') {
      use_as_query_n = i;
    }
    for (int k = 1; argv[i][k] != '\0' && argv[i][0] == '-'; k++) {
      has_query_as_e_f_option |= (argv[i][k] == 'e' || argv[i][k] == 'f');
    }
  }
  if (use_as_query_n == 0 && has_query_as_e_f_option == 0) {
    arguments = -1;
    printUsage();
  } else {
    use_as_query_n *= !has_query_as_e_f_option;
  }

  temppath_query = allocateTempFile();
  if (temppath_query != NULL) {
    query_file = fopen(temppath_query, "w");
  } else {
    arguments = -1;
  }
  for (int i = 1; i < argc && arguments >= 0; i++) {
    if (i != use_as_query_n && argv[i][0] != '-') {
      files_names[i_files_to_process] = (argv[i]);
      i_files_to_process++;
    } else if (argv[i][0] != '-') {
      fputs(buffer = setupQuery(argv[use_as_query_n]), query_file);
      free(buffer);
      q_i++;
    }
    for (unsigned int k = 1;
         arguments >= 0 && argv[i][0] == '-' && argv[i][k] != '\0'; k++) {
      t = (argv[i][k + 1] == '\0');
      if ((i + t) < argc) {
        switch (argv[i][k]) {
          case 'e':
            i += t;
            buffer = setupQuery((argv[i] + ((k + 1) * !t)));
            fputs(buffer, query_file);
            fputs("\n", query_file);
            free(buffer);
            k = (t == 0) ? (strlen(argv[i]) - 1) : k;
            break;
          case 'f':
            i += t;
            arguments |=
                loadQueryFileFromAnother(query_file, argv[i] + ((k + 1) * !t));
            k = (t == 0) ? strlen(argv[i]) - 1 : k;
            break;
          default:
            arguments |= argumentsWrite(argv[i][k]);
        }
      } else {
        printf("grep: option requires an argument -- %c\n", argv[i][k]);
        printUsage();
        arguments = -1;
      }
    }
  }

  if (query_file != NULL) {
    fclose(query_file);
    query_file = fopen(temppath_query, "r");
  }

  if (argc > 1 && arguments >= 0) {
    reegex = setupReegex(query_file, arguments);
  } else {
    arguments |= -1;
    reegex = setupReegex(NULL, 0);
  }
  if (query_file != NULL) {
    fclose(query_file);
    remove(temppath_query);
  }
  free(temppath_query);

  if (i_files_to_process <= 1) {
    arguments |= NO_FILENAME_OUTPUT;
  }
  if (i_files_to_process == 0 && arguments >= 0) {
    fileHandler(arguments, reegex, stdin, "stdin");
  }

  for (int i = 0; i < i_files_to_process && arguments >= 0; i++) {
    if ((stream = fopen(files_names[i], "r")) != NULL) {
      fileHandler(arguments, reegex, stream, files_names[i]);
      fclose(stream);
    } else if (SUPPRESS_FILENAME_ERRORS ^ arguments) {
      printf("s21_grep: %s: No such file or directory\n", files_names[i]);
    }
  }
  regfree(&reegex);
}

regex_t setupReegex(FILE* query_file, const int arguments) {
  regex_t reegex;
  char reegex_format[2048 * 16] = "\0";
  char* buffer = NULL;
  size_t len = 0;
  int reg_flags = REG_EXTENDED;

  if (query_file != NULL) {
    while (getLineAndAlloc(&buffer, &len, query_file) != -1) {
      if (reegex_format[0] != '\0') {
        strcat(reegex_format, "|");
      }
      buffer[strlen(buffer) - 1] = '\0';
      strcat(reegex_format, buffer);
    }
  }
  if (buffer != NULL) {
    free(buffer);
    buffer = NULL;
  }
  if (arguments & CASE_INSENSITIVE) {
    reg_flags |= REG_ICASE;
  }

  regcomp(&reegex, reegex_format, reg_flags);
  return reegex;
}

// the catcoction that sanitizes query in a way that it acts like grep one.
// cursed
char* setupQuery(const char* query) {
  char prev = '\0';
  char *buffer, *buffer2 = NULL;
  char checking[][1] = {"(", ")", "{", "}", "?", "+", "|", "^", "$", "\0"};
  buffer = strduplicate(query);
  if (buffer == NULL) {
    while (buffer == NULL) {
      buffer = malloc(sizeof(char));
    }
    buffer[0] = '\0';
  }

  for (int i = 0; checking[i][0] && buffer != NULL; i++) {
    for (int j = 0; buffer[j]; j++) {
      if (buffer[j] == checking[i][0] && prev != '\\') {
        buffer2 = strduplicate(buffer + j);
        buffer[j] = '\0';
        strcat(buffer, "\\");
        strcat(buffer, buffer2);
        if (buffer2 != NULL) {
          free(buffer2);
        }
        j++;
      }
      if (buffer[j] == checking[i][0] && prev == '\\') {
        buffer2 = strduplicate(buffer + j);
        buffer[j - 1] = '\0';
        strcat(buffer, buffer2);
        j--;
        if (buffer2 != NULL) {
          free(buffer2);
        }
      }
      prev = buffer[j];
    }
  }
  return buffer;
}

// handeles line search and printing trough a file
int fileHandler(const int arguments, regex_t reegex, FILE* stream,
                char* filename) {
  size_t buffer_size = 0;
  int match_count = 0;

  char* buffer = NULL;
  for (int line_i = 0; getLineAndAlloc(&buffer, &buffer_size, stream) > 0;
       line_i++) {
    if (buffer != NULL) {
      match_count +=
          handleLineWithRegex(buffer, filename, line_i, reegex, arguments);
    }
  }
  free(buffer);

  if (arguments & OUTPUT_COUNT && !(arguments & NO_FILENAME_OUTPUT)) {
    printf("%s:", filename);
  }
  if (!(arguments & MATCHING_FILES_ONLY) && arguments & OUTPUT_COUNT) {
    printf("%d\n", match_count);
  }
  if (arguments & MATCHING_FILES_ONLY && arguments & OUTPUT_COUNT) {
    printf("%d\n", match_count > 0);
  }
  if (arguments & MATCHING_FILES_ONLY && match_count > 0) {
    printf("%s\n", filename);
  }
  return match_count;
}

int handleLineWithRegex(char* buffer, char* filename, int line_i,
                        regex_t reegex, int arguments) {
  regmatch_t __pmatch[4];
  int rv = 0;
  char* buffer_2;
  while (regexec(&reegex, buffer, 2, __pmatch, 0) ==
         ((arguments & INVERT_MATCH) != 0)) {
    buffer_2 = strduplicate(buffer);

    if ((arguments & ONLY_MATCHING_PARTS_LINE) && !(arguments & INVERT_MATCH)) {
      strRip(&buffer, __pmatch->rm_eo, ((size_t)0) - 1);
      strRip(&buffer_2, __pmatch->rm_so, __pmatch->rm_eo);
    } else {
      buffer[0] = '\0';
    }
    if ((arguments & OUTPUT_COUNT) == 0 &&
        (MATCHING_FILES_ONLY & arguments) == 0) {
      if ((arguments & NO_FILENAME_OUTPUT) == 0) {
        printf("%s:", filename);
      }
      if (arguments & PROCEED_LINE_NUM) {
        printf("%d:", line_i + 1);
      }
      printf("%s", buffer_2);
    }

    rv++;
    free(buffer_2);
    arguments = arguments ^ (arguments & INVERT_MATCH);
  }
  return rv != 0;
}

long long getLineAndAlloc(char** destination, size_t* size_of_destination,
                          FILE* stream) {
  char c = 'c';
  char* destination_new_ptr;
  size_t i = 0;
  if (stream == NULL) {
    c = '\n';
    i = 1;
  }
  if (destination != NULL) {
    if (*destination == NULL) {
      (*destination) = malloc(sizeof(char));
    }
  }
  for (; c != '\n' && c != EOF && destination != NULL &&
         size_of_destination != NULL;
       i++) {
    if (i < CHAR_MAX) {
      c = fgetc(stream);
      *size_of_destination = sizeof(char) * 8 * (i / 8 + ((i / 8) == 0));
      destination_new_ptr = realloc(*destination, *size_of_destination);
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

int argumentsWrite(char argument) {
  int rv = 0;
  switch (argument) {
    case 'l':
      rv = MATCHING_FILES_ONLY;  // 0000 0001 for l
      break;
    case 'i':
      rv = CASE_INSENSITIVE;  // 0000 0010 for i
      break;
    case 'v':
      rv = INVERT_MATCH;  // 0000 0100 for v
      break;
    case 'c':
      rv = OUTPUT_COUNT;  // 0000 1000 for c
      break;
    case 'h':
      rv = NO_FILENAME_OUTPUT;  // 0001 0000 for h
      break;
    case 'n':
      rv = PROCEED_LINE_NUM;  // 0010 0000 for n
      break;
    case 's':
      rv = SUPPRESS_FILENAME_ERRORS;  // 0100 0000 for s
      break;
    case 'o':
      rv = ONLY_MATCHING_PARTS_LINE;  // 1000 0000 for o
      break;
    case 'e':
    case 'f':
      break;
    default:
      rv = -1;  // invalid option
      printf("s21_grep: invalid option -- %c\n", argument);
      printUsage();
      break;
  }
  return rv;
}

void printUsage(void) {
  printf(
      "usage: s21_grep [-c] [-e pattern] [-f file] [-i] [-v] [-l] [-o] [-s] "
      "[-h] [-n] [pattern] [file ...]\n");
}

int loadQueryFileFromAnother(FILE* dest, const char* file_with_query_name) {
  char* buffer = NULL;
  char* buffer_2;
  size_t len = 0;
  FILE* stream;
  int rv = 0;
  stream = fopen(file_with_query_name, "r");
  if (stream != NULL) {
    while (getLineAndAlloc(&buffer, &len, stream) != -1) {
      buffer_2 = setupQuery(buffer);
      for (int i = 0; buffer_2[i] != '\0'; i++) {
        putc(buffer_2[i], dest);
      }
      if (buffer_2 != NULL) {
        free(buffer_2);
      }
    }
    free(buffer);
    fclose(stream);
  } else {
    rv = -1;
  }
  return rv;
}

char* strduplicate(const char* buffer) {
  char* output = NULL;
  output = malloc(sizeof(*buffer));
  if (output != NULL) {
    strcpy(output, buffer);
  }
  return output;
}

char* allocateTempFile() {
  char* temppath_query = strduplicate("./temp/s21_grep_temp_0");
  system("mkdir -p ./temp");
  if (strlen(temppath_query) + 16 < 261) {
    for (int i = 1; open(temppath_query, O_CREAT | O_WRONLY | O_EXCL,
                         S_IRUSR | S_IWUSR) == -1;
         i++) {
      snprintf(temppath_query, sizeof(char) * 261, "./temp/s21_grep_temp_%d",
               i);
    }
  } else {
    printf("grep: Error creating temporary file");
  }

  return temppath_query;
}
