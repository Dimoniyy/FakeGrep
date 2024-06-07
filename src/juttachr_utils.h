#ifndef size_t 
#define size_t long unsigned int
#endif
#ifndef NULL
#define NULL ((void *)0)
#endif
#include <stdio.h>

extern char* strduplicate(const char* buffer);
extern void strRip(char** target_ptr, const size_t from, const size_t to);