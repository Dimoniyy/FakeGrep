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
#define COMBINATIONS_MAX_LENGHT 10
#define BUFFER_MAX_LENGHT 1024

int combinations(const char* letters, char* temp_res, unsigned long int from,
                 unsigned long int until, unsigned long int iteration,
                 unsigned long int target_lenght, char** out);
int filesCompare(char* file_1_path, char* file_2_path);
void runCompareGrep(char*);
char* strduplicate(const char* buffer);
void runCompareCat(char* flags_wo_dash);

int main() {
  int how_many = 0, fail = 0, res = 0;
  char buffer[BUFFER_MAX_LENGHT];
  char** out;
  int len_of_letters;
  FILE* log_file;
#ifndef TEST_CAT
  char letters[] = "livchnso";  // 8 letters
  char sys_tempfilepath[] = "./temp/tempfile_test_grep";
  char s21_tempfilepath[] = "./temp/tempfile_test_s21grep";
  char log_file_path[] = "./log_file_grep";
  void (*runTests)(char*) = runCompareGrep;
#else
  char letters[] = "bevnst";
  char sys_tempfilepath[] = "./temp/tempfile_test_cat";
  char s21_tempfilepath[] = "./temp/tempfile_test_s21cat";
  char log_file_path[] = "./log_file_cat";
  void (*runTests)(char*) = runCompareCat;
#endif
  out = malloc(sizeof(char*) * BUFFER_MAX_LENGHT * BUFFER_MAX_LENGHT);
  log_file = fopen(log_file_path, "w");
  len_of_letters = strlen(letters);
  for (int i = 1; i < len_of_letters; i++) {
    how_many = combinations(letters, buffer, 0, len_of_letters, 0, i, out);
  }
  for (int j = 0; j < how_many; j++) {
    runTests(out[j]);
    fail += (filesCompare(sys_tempfilepath, s21_tempfilepath) != 0);
    if (fail) {
      fprintf(log_file, "fail with -%s\n", out[j]);
      fflush(log_file);
    }
    res += fail;
  }
  for (int j = 0; j < how_many; j++) {
    free(out[j]);
  }
  free(out);
  printf("%d fail(s)\n", res);
  if (res > 0) {
    printf("check %s for details\n", log_file_path);
  }
  snprintf(buffer, BUFFER_MAX_LENGHT, "rm %s", sys_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT, "rm %s", s21_tempfilepath);
  system(buffer);
}

void runCompareGrep(char* flags_wo_dash) {
  char sys_tempfilepath[] = "./temp/tempfile_test_grep";
  char s21_tempfilepath[] = "./temp/tempfile_test_s21grep";
  system("mkdir -p ./temp");
  char buffer[BUFFER_MAX_LENGHT];

  snprintf(buffer, BUFFER_MAX_LENGHT, "grep -%s foo text > %s", flags_wo_dash,
           sys_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT, "./grep/s21_grep -%s foo text > %s",
           flags_wo_dash, s21_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT, "grep -%se foo text >> %s", flags_wo_dash,
           sys_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT, "./grep/s21_grep -%se foo text >> %s",
           flags_wo_dash, s21_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT, "grep -%sf \"text 2\" text >> %s",
           flags_wo_dash, sys_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT,
           "./grep/s21_grep -%sf \"text 2\" text >> %s", flags_wo_dash,
           s21_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT, "grep -%sefoo text >> %s", flags_wo_dash,
           sys_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT, "./grep/s21_grep -%sefoo text >> %s",
           flags_wo_dash, s21_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT, "grep \"-%sftext 2\" text >> %s",
           flags_wo_dash, sys_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT,
           "./grep/s21_grep \"-%sftext 2\" text >> %s", flags_wo_dash,
           s21_tempfilepath);
  system(buffer);
}

#ifdef TEST_CAT
void runCompareCat(char* flags_wo_dash) {
  char sys_tempfilepath[] = "./temp/tempfile_test_cat";
  char s21_tempfilepath[] = "./temp/tempfile_test_s21cat";
  system("mkdir -p ./temp");
  char buffer[BUFFER_MAX_LENGHT];

  snprintf(buffer, BUFFER_MAX_LENGHT, "cat -%s text \"text 2\" > %s",
           flags_wo_dash, sys_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT, "./cat/s21_cat -%s text \"text 2\" > %s",
           flags_wo_dash, s21_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT, "cat -%s text >> %s", flags_wo_dash,
           sys_tempfilepath);
  system(buffer);
  snprintf(buffer, BUFFER_MAX_LENGHT, "./cat/s21_cat -%s text >> %s",
           flags_wo_dash, s21_tempfilepath);
  system(buffer);
}
#endif

int filesCompare(char* file_1_path, char* file_2_path) {
  FILE* file1;
  FILE* file2;
  int file_1_out;
  int file_2_out;
  int res = 0;
  file1 = fopen(file_1_path, "r");
  file2 = fopen(file_2_path, "r");
  if (file1 != NULL && file2 != NULL) {
    do {
      file_1_out = fgetc(file1);
      file_2_out = fgetc(file2);
      res += (file_1_out != file_2_out);
    } while ((file_1_out != EOF) && (file_2_out != EOF));
  } else {
    perror(NULL);
  }
  fclose(file1);
  fclose(file2);
  return res;
}

int combinations(const char* letters, char* temp_res, unsigned long int from,
                 unsigned long int until, unsigned long int iteration,
                 unsigned long int target_lenght, char** out) {
  static int out_n = 0;
  if (iteration == target_lenght &&
      out_n < (BUFFER_MAX_LENGHT * BUFFER_MAX_LENGHT)) {
    temp_res[target_lenght] = '\0';
    out[out_n] = strduplicate(temp_res);
    out_n++;
  }
  unsigned long int i = from;
  while (i < strlen(letters) &&
         from + target_lenght - iteration <= strlen(letters) &&
         iteration < target_lenght) {
    temp_res[iteration] = letters[i];
    combinations(letters, temp_res, i + 1, until, iteration + 1, target_lenght,
                 out);
    i++;
  }
  return out_n;
}

char* strduplicate(const char* buffer) {
  char* output = NULL;
  output = malloc((strlen(buffer) + 1) * sizeof(char));
  if (output != NULL) {
    strcpy(output, buffer);
  }
  return output;
}
