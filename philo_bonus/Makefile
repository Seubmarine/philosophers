NAME = philo_bonus
CC = gcc
CFLAGS = -pthread -g3

SRCS = main.c
OBJS = $(SRCS:%.c=%.o)

$(NAME): $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

clean:
	-rm $(OBJS)

fclean : clean
	-rm $(NAME)

re: fclean all

.PHONY: all clean fclean re