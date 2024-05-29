CC=gcc
clang_format_path= ../materials/linters/.clang-format
CFLAGS= -Wall -Wextra -Werror -std=c11 -g
grep=./grep
cat=./cat
ARGS=
LEAKS=leaks
GREPARGS=Nya text


clang-format:
	cp $(clang_format_path) $(cat)
	clang-format -i $(cat)/*.c
	clang-format -i $(cat)/*.h
	rm ./cat/.clang-format
	cp $(clang_format_path) $(grep)
	clang-format -i $(grep)/*.c
	clang-format -i $(grep)/*.h
	rm ./grep/.clang-format
	cp $(clang_format_path) ./
	clang-format -i ./*.c
	rm ./.clang-format

cat_test: s21_cat
	$(cat)/s21_cat $(ARGS)

s21_cat:
	make -C $(cat)

s21_grep:
	make -C $(grep)

leaks_grep: s21_grep
	$(LEAKS) --atExit -- $(grep)/s21_grep $(ARGS)

test: s21_grep
	$(grep)/s21_grep $(ARGS)
	grep $(ARGS)
