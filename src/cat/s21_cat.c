#include "./s21_cat.h"

int main(int argc, char* argv[]) {
  int arguments = 0x0, i = 1;
  for (; i < argc && argv[i][0] == '-'; i++) {
    arguments = arguments | argumentsWrite(argv[i]);
  }

  do {
    FILE* stream;
    if (!(arguments & USE_STDIN) && i < argc) {
      stream = fopen(argv[i], "r");
    } else {
      stream = stdin;
      i = argc;
    }
    if (!stream) {
      printf("cat: %s: No such file or directory\n", argv[i]);
    } else {
      fileHandlerCat(stream, arguments);
    }
    i++;
    if (stream != NULL) fclose(stream);
  } while (i < argc);
}

void fileHandlerCat(FILE* stream, const int arguments) {
  char c = 'c', c_prev = '\0', c_prev_prev = '\0';
  for (int line = 1; c != EOF;) {
    do {
      c = getc(stream);
    } while (arguments & SQUEEZE_MULTIPLE_AJACENT_EMPTY_LINES && c == '\n' &&
             c_prev == '\n' && c_prev_prev == '\n');

    if ((arguments & NUMBER_LINES_NO_EMPTY &&
         (c != '\n' && (c_prev == '\n' || c_prev == '\0') && c != EOF)) ||
        (arguments & NUMBER_LINES && !(arguments & NUMBER_LINES_NO_EMPTY) &&
         (c_prev == '\n' || c_prev == '\0') && c != EOF)) {
      for (int k = 100000; k > line && k > 9; k /= 10) {
        printf(" ");
      }
      printf("%d\t", line);
      line++;
    }
    c = convertInvisible(arguments, c);

    if (arguments & EOL_DOLLAR_SIGN && c == '\n') {
      if (arguments & NUMBER_LINES_NO_EMPTY && c_prev == '\n') {
        printf("  ");
      }
      printf("$");
    }
    if (c != EOF) {
      printf("%c", c);
    }

    c_prev_prev = c_prev;
    c_prev = c;
  }
}

char convertInvisible(const int arguments, char c) {
  if (c != 0xA && arguments & DISPLAY_NON_PRINTING &&
      c < DISPLAY_TAB_CHARACTERS && c > 0x0 &&
      (c != 0x9 || arguments & DISPLAY_TAB_CHARACTERS)) {
    c ^= (0x40 * (c != 0x7F && c != '\n'));
    printf("^");
  }
  return c;
}

int argumentsWrite(char* argument) {
  int rv = 0;
  switch (argument[1]) {
    case 'b':
      rv |= NUMBER_LINES_NO_EMPTY;
      break;
    case 'e':
      rv |= EOL_DOLLAR_SIGN | DISPLAY_NON_PRINTING;
      break;
    case 'n':
      rv |= NUMBER_LINES;
      break;
    case 's':
      rv |= SQUEEZE_MULTIPLE_AJACENT_EMPTY_LINES;
      break;
    case 't':
      rv |= DISPLAY_TAB_CHARACTERS | DISPLAY_NON_PRINTING;
      break;
    case 'E':
      rv |= EOL_DOLLAR_SIGN;
      break;
    case 'T':
      rv |= DISPLAY_TAB_CHARACTERS;
      break;
    case 'u':
      //
      break;
    case 'v':
      rv |= DISPLAY_NON_PRINTING;
      break;
    case '\0':
      rv |= USE_STDIN;
      break;
    default:
      rv |= argumentsLongWriting(argument);
      if (!rv) {
        printf("cat: illegal option -- %c\nusage: cat [-belnstuv] [file ...]\n",
               argument[1]);
      }
      break;
  }
  return rv;
}

int argumentsLongWriting(char* argument) {
  int rv = 0;
  if (!strcmp(argument, "--number-nonblank")) {
    rv = NUMBER_LINES_NO_EMPTY;
  } else if (!strcmp(argument, "--number")) {
    rv = NUMBER_LINES;
  } else if (!strcmp(argument, "--squeeze-blank")) {
    rv = SQUEEZE_MULTIPLE_AJACENT_EMPTY_LINES;
  }
  return rv;
}