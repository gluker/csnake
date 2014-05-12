NAME = snake
main:
	gcc $(NAME).c -g -Wall -O3 -std=c99 -lncurses -o $(NAME)

run:
	 ./$(NAME)
clean:
	rm ./$(NAME)
