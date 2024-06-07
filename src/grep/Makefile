NAME=s21_grep
CC=gcc
CFLAGS= -Wall -Werror -Wextra -std=c11 -g

$(NAME): $(NAME).c
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).c

clean: 
	rm $(NAME)

rebuild: clean $(NAME)

