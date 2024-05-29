cat < (diff <(cat -n text) <(./s21_cat -b text))
cat < (diff <(cat -b text) <(./s21_cat -b text))