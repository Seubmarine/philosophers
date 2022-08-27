/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbousque <tbousque@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/27 08:23:17 by tbousque          #+#    #+#             */
/*   Updated: 2022/08/27 10:57:57 by tbousque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

typedef struct s_arg_info
{
	size_t philo_count;
	int time_to_die;
	int time_to_eat;
	int time_to_sleep;
	pthread_mutex_t can_print;
	struct timeval time_begin;
} t_arg_info;

typedef enum e_philo_state
{
	EAT,
	THINK,
	SLEEP,
	DEAD,
	FORK1,
	FORK2
} t_philo_state;

typedef struct s_philo
{
	size_t index;
	pthread_mutex_t fork_left;
	pthread_mutex_t *fork_right;
	t_philo_state state;
	t_arg_info *input;
	unsigned long long last_eaten;
	// struct  timeval last_eaten;
} t_philo;

t_arg_info get_arg_info(char const *argv[])
{
	t_arg_info arg;

	arg.philo_count = atoi(argv[1]);
	arg.time_to_die = atoi(argv[2]);
	arg.time_to_eat = atoi(argv[3]);
	arg.time_to_sleep = atoi(argv[4]);
	gettimeofday(&arg.time_begin, NULL);
	if (pthread_mutex_init(&arg.can_print, NULL) != 0)
		printf("Can print can't init\n");
	return (arg);
}


int main(int argc, char const *argv[])
{
	t_arg_info arg;
	size_t i;
	t_philo *philos;

	if (argc < 5 || argc > 6)
		return (EXIT_FAILURE);
	arg = get_arg_info(argv);
	// philos = malloc(sizeof(*philos) * arg.philo_count);
	// if (philos == NULL)
	// 	return (EXIT_FAILURE);
	// i = 0;
	// pthread_t *threads = malloc(sizeof(*threads) * arg.philo_count);

	int error;// = sem_unlink("philo_forks");
	sem_t *parent_sem = sem_open("philo_forks", O_CREAT);	
	if (!parent_sem)
	{ 
		perror("Sem_open");
		exit(EXIT_FAILURE);
	}
	error = sem_init(parent_sem, 1, arg.philo_count);
	pid_t process = fork();
	perror("Parent");
	if (process == 0) //I'm a child / I'm a philosopher
	{
		printf("Youpi\n");
		sem_t *myfork = parent_sem;//sem_open("philo_forks", O_RDWR);
		perror("Philo");
		int value;
		error = sem_getvalue(myfork, &value);
		printf("I'm a child error: %i, value %i\n", error, value);
		error = sem_wait(myfork);
		error = sem_getvalue(myfork, &value);
		printf("sem_wait error: %i, value %i\n", error, value);
		error = sem_close(myfork);
		printf("sem_close\n");
		exit(EXIT_SUCCESS);
	}
	waitpid(process, NULL, 0);
	error = sem_destroy(parent_sem);
	error = sem_unlink("philo_forks");
	return (EXIT_SUCCESS);
}
