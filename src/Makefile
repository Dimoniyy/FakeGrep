CC=gcc
clang_format_path= ../materials/linters/.clang-format
CFLAGS= -Wall -Wextra -Werror -std=c11 -g
grep=./grep
cat=./cat
ARGS=
LEAKS=leaks

all: s21_cat s21_grep clang-format

clang-format:
	cp $(clang_format_path) $(cat)
	cp $(clang_format_path) $(grep)
	cp $(clang_format_path) ./
	clang-format -i $(cat)/*.c
	clang-format -i $(cat)/*.h
	clang-format -i $(grep)/*.c
	clang-format -i $(grep)/*.h
	clang-format -i ./*.c
	rm -f ./grep/.clang-format
	rm -f ./cat/.clang-format
	rm -f ./.clang-format

cat_test: s21_cat
	$(cat)/s21_cat $(ARGS)
	echo cat
	cat $(ARGS)

s21_cat:
	make -C $(cat)

s21_grep:
	make -C $(grep)

leaks_grep: s21_grep
	$(LEAKS) --atExit -- $(grep)/s21_grep $(ARGS)

test_grep: s21_grep
	$(CC) $(CFLAGS) -o $@ test.c
	./test_grep

test_cat: s21_cat
	$(CC) $(CFLAGS) -D TEST_CAT=1 -o $@ test.c
	./test_cat

test_both: test_grep test_cat

clean:
	rm -f -- test test_cat test_grep a.out ./grep/s21_grep ./cat/s21_cat