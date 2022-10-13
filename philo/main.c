/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbousque <tbousque@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 22:45:26 by tbousque          #+#    #+#             */
/*   Updated: 2022/10/13 09:46:30 by tbousque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

t_arg_info	get_arg_info(int argc, char const *argv[])
{
	t_arg_info	arg;

	arg.philo_count = atoi(argv[1]);
	arg.time_to_die = atoi(argv[2]);
	arg.time_to_eat = atoi(argv[3]);
	arg.time_to_sleep = atoi(argv[4]);
	arg.simulation_stop = 0;
	arg.cycle_count = 0;
	gettimeofday(&arg.time_begin, NULL);
	if (argc == 6)
	{
		arg.cycle_count = atoi(argv[5]);
		if (arg.cycle_count <= 0)
			exit(EXIT_SUCCESS);
	}
	if (pthread_mutex_init(&arg.can_print, NULL) != 0)
		printf("Can print can't init\n");
	return (arg);
}

void *philo_start_one(void *arg)
{
	t_philo *const	self = arg;

	self->state = THINK;
	print_state(self);
	self->state = FORK1;
	pthread_mutex_lock(self->fork_right);
	print_state(self);
	usleep(time_to_do(self, get_ms(self->input->time_begin), self->input->time_to_die) * 1000);
	self->state = DEAD;
	pthread_mutex_unlock(self->fork_right);
	print_state(self);
	return (NULL);
}

int	main(int argc, char const *argv[])
{
	t_arg_info	arg;
	size_t		i;
	t_philo		*philos;

	if (argc < 5 || argc > 6)
		return (EXIT_FAILURE);
	arg = get_arg_info(argc, argv);
	philos = malloc(sizeof(*philos) * arg.philo_count);
	if (philos == NULL)
		return (EXIT_FAILURE);
	i = 0;
	pthread_t *threads = malloc(sizeof(*threads) * arg.philo_count);
	unsigned long long last_eaten = get_ms(arg.time_begin);
	while (i < arg.philo_count - 1)
	{
		pthread_mutex_init(&philos[i].fork_left, NULL);
		philos[i].input = &arg;
		philos[i].fork_right = &(philos[i + 1].fork_left);
		philos[i].index = i + 1;
		philos[i].last_eaten = last_eaten;
		i++;
	}
	pthread_mutex_init(&philos[i].fork_left, NULL);
	philos[i].input = &arg;
	philos[i].fork_right = &(philos[0].fork_left);
	philos[i].index = i + 1;
	philos[i].last_eaten = last_eaten;
	if (arg.philo_count == 1)
		pthread_create(threads + i, NULL, philo_start_one, &philos[i]);
	else if(philos_pthread_create(philos, threads, arg.philo_count, philo_start))
	{
		pthread_mutex_lock(&arg.can_print);
		(void)!write(STDERR_FILENO, "Error creating thread, simulation will stop\n", 44);
		arg.simulation_stop = 1;
		pthread_mutex_unlock(&arg.can_print);
	}
	i = 0;
	while (i < arg.philo_count)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	free(philos);
	free(threads);
	return (EXIT_SUCCESS);
}
