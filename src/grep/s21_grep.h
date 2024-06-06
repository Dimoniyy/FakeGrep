#ifndef C3_SIMPLEBASHUTILS_GREP_HEADERS_S21_GREP_H_
#define C3_SIMPLEBASHUTILS_GREP_HEADERS_S21_GREP_H_

#include <fcntl.h>
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MATCHING_FILES_ONLY 0x1        // -l
#define CASE_INSENSITIVE 0X2           // -i
#define INVERT_MATCH 0x4               // -v
#define OUTPUT_COUNT 0X8               // -c
#define NO_FILENAME_OUTPUT 0x10        // -h
#define PROCEED_LINE_NUM 0x20          // -n
#define SUPPRESS_FILENAME_ERRORS 0x40  // -s
#define ONLY_MATCHING_PARTS_LINE 0x80  // -o
#define TEMP_DIR "./temp/"

extern int fileHandler(const int, regex_t, FILE*, char*);
extern int argumentsWrite(char);
extern regex_t setupReegex(FILE*, const int);
extern void printUsage(void);
extern char* setupQuery(const char* query);
extern int loadQueryFileFromAnother(FILE*, const char*);
extern char* strduplicate(const char*);
extern char* allocateTempFile();
extern void strRip(char** target_ptr, const size_t from, const size_t to);
extern long long getLineAndAlloc(char** destination, FILE* stream);
extern int handleLineWithRegex(char* buffer, char* filename, int line_i,
                               regex_t reegex, int arguments);

#endif
