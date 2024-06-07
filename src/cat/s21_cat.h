#pragma once

#include <stdio.h>
#include <string.h>
#define BUFFER_LIMIT 2048
#define NUMBER_LINES_NO_EMPTY 0x1                  // -b GNU: --number-nonblank
#define EOL_DOLLAR_SIGN 0x2                        // -e
#define DISPLAY_NON_PRINTING 0x4                   // -v
#define NUMBER_LINES 0x8                           // -n --nonumber
#define SQUEEZE_MULTIPLE_AJACENT_EMPTY_LINES 0x10  //-s
#define DISPLAY_TAB_CHARACTERS 0x20                //-T -t
#define USE_STDIN 0x40

extern int argumentsWrite(char*);
extern int argumentsLongWriting(char*);
extern void fileHandlerCat(FILE*, const int);
extern char convertInvisible(const int, char);