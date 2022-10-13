/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbousque <tbousque@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 22:45:26 by tbousque          #+#    #+#             */
/*   Updated: 2022/10/13 10:23:44 by tbousque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

//0 on success, 1 on error
int	get_arg_info(int argc, char const *argv[], t_arg_info *arg)
{
	arg->philo_count = atoi(argv[1]);
	arg->time_to_die = atoi(argv[2]);
	arg->time_to_eat = atoi(argv[3]);
	arg->time_to_sleep = atoi(argv[4]);
	arg->simulation_stop = 0;
	arg->cycle_count = 0;
	gettimeofday(&arg->time_begin, NULL);
	if (argc == 6)
	{
		arg->cycle_count = atoi(argv[5]);
		if (atoi(argv[5]) <= 0)
			return (1);
	}
	if (pthread_mutex_init(&arg->can_print, NULL) != 0)
	{
		printf("Mutext \"can_print\" can't init\n");
		return (1);
	}
	if (atoi(argv[1]) <= 0|| arg->time_to_eat < 0 || arg->time_to_die < 0 \
		|| arg->time_to_sleep < 0)
		return (1);
	return (0);
}

void	*philo_start_one(void *arg)
{
	t_philo *const	self = arg;

	self->state = THINK;
	print_state(self);
	self->state = FORK1;
	pthread_mutex_lock(self->fork_right);
	print_state(self);
	usleep(time_to_do(self, get_ms(self->input->time_begin), \
		self->input->time_to_die) * 1000);
	self->state = DEAD;
	pthread_mutex_unlock(self->fork_right);
	print_state(self);
	return (NULL);
}

//return the number of mutex that failed, 0 on success
size_t philos_mutex_init(t_philo *philos, size_t philo_count)
{
	size_t	i;

	i = 0;
	while (i < philo_count)
	{
		if(pthread_mutex_init(&philos[i].fork_left, NULL))
			break;
		i++;
	}
	if (i < philo_count)
	{
		philo_count = i + 1;
		i = 0;
		while (i < philo_count)
		{
			pthread_mutex_destroy(&philos[i].fork_left);
			i++;
		}
		return (philo_count);
	}
	return (0);
}

int	main(int argc, char const *argv[])
{
	t_arg_info	arg;
	t_philo		*philos;
	pthread_t	*threads;

	if (argc < 5 || argc > 6 || get_arg_info(argc, argv, &arg))
		return (EXIT_FAILURE);
	philos = malloc(sizeof(*philos) * arg.philo_count);
	if (philos == NULL)
		return (EXIT_FAILURE);
	if (philos_mutex_init(philos, arg.philo_count))
	{
		free(philos);
		return (EXIT_FAILURE);
	}
	threads = malloc(sizeof(*threads) * arg.philo_count);
	if (threads == NULL)
	{
		free(philos);
		exit(EXIT_FAILURE);
	}
	size_t	i = 0;
	unsigned long long last_eaten = get_ms(arg.time_begin);
	while (i < arg.philo_count - 1)
	{
		philos[i].input = &arg;
		philos[i].fork_right = &(philos[i + 1].fork_left);
		philos[i].index = i + 1;
		philos[i].last_eaten = last_eaten;
		i++;
	}
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
