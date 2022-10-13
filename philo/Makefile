NAME = philo
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pthread -g3 -O3

SRCS = main.c time_helper.c philos_pthread_create.c philo_routine.c philo_print.c
OBJS = $(SRCS:%.c=%.o)

$(NAME): $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o philo

all: $(NAME)

clean:
	-rm $(OBJS)

fclean : clean
	-rm $(NAME)

re: fclean all

.PHONY: all clean fclean re