/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbousque <tbousque@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 22:45:26 by tbousque          #+#    #+#             */
/*   Updated: 2022/10/12 23:20:05 by tbousque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

t_arg_info get_arg_info(int argc, char const *argv[])
{
	t_arg_info arg;

	arg.philo_count = atoi(argv[1]);
	arg.time_to_die = atoi(argv[2]);
	arg.time_to_eat = atoi(argv[3]);
	arg.time_to_sleep = atoi(argv[4]);
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

void print_state(t_philo *const self)
{
	const char *state_string[6] = {"%lli %lu is eating\n",
							 "%lli %lu is thinking\n",
							 "%lli %lu is sleeping\n",
							 "%lli %lu died\n",
							 "%lli %lu has taken a fork\n",
							 "%lli %lu has taken a fork\n"};
	
	pthread_mutex_lock(&self->input->can_print);
	unsigned long long time = get_ms(self->input->time_begin);
	if (has_died(self, time))
	 	self->state = DEAD;
	printf(state_string[self->state], time, self->index);
	if (self->state == FORK2)
	{
		self->state = EAT;
		printf(state_string[self->state], time, self->index);
	}
	pthread_mutex_unlock(&self->input->can_print);
}

int philo_has_one_fork(t_philo *const self, pthread_mutex_t *current_fork, int fork_pos)
{
	unsigned long long current_time;
	t_philo_state previous_state = self->state;
	current_time = get_ms(self->input->time_begin);
	if (has_died(self, current_time))
	{
		print_state(self);
		return (0);
	}
	pthread_mutex_lock(current_fork);
	self->state = FORK1 + fork_pos;
	print_state(self);
	if (self->state == DEAD)
	{
		pthread_mutex_unlock(current_fork);
		return (0);
	}
	self->state = previous_state;
	return (1);
}

int	philo_has_two_fork_in_time(t_philo *const self, pthread_mutex_t *a, pthread_mutex_t *b)
{
	if (philo_has_one_fork(self, a, 0))
	{
		if (philo_has_one_fork(self, b, 1))
			return (1);
		pthread_mutex_unlock(a);
		return (0);
	}
	return (0);
}

int philo_eat(t_philo *const self)
{
	self->state = DEAD;
	if (self->index % 2)
	{
		if (philo_has_two_fork_in_time(self, &self->fork_left, self->fork_right))
			self->state = EAT;
	}
	else
	{
		if (philo_has_two_fork_in_time(self, self->fork_right, &self->fork_left))
			self->state = EAT;
	}
	if (self->state == DEAD)
		return (0);
	self->last_eaten = get_ms(self->input->time_begin);
	usleep(time_to_do(self, get_ms(self->input->time_begin), self->input->time_to_eat) * 1000);
	pthread_mutex_unlock(self->fork_right);
	pthread_mutex_unlock(&self->fork_left);
	return (1);
}

int philo_sleep(t_philo *const self)
{
	self->state = SLEEP;
	print_state(self);
	if (self->state == DEAD)
		return (0);
	
	unsigned long long time_to_do_v = time_to_do(self, get_ms(self->input->time_begin), self->input->time_to_sleep);
	usleep(time_to_do_v * 1000);
	if (has_died(self, get_ms(self->input->time_begin)))
	{
		self->state = DEAD;
		print_state(self);
		return (0);
	}
	return (1);
}

void *philo_start(void *arg)
{
	t_philo *const self = arg;
	while (1)
	{
		if (self->input->cycle_count)
			if (self->input->cycle_count <= self->cycle_current)
				break ;
		self->cycle_current++;
		self->state = THINK;
		print_state(self);
		if (self->input->philo_count % 2 == 1)
			usleep(200);
		if (self->state == DEAD)
			return (NULL);
		if (!philo_eat(self))
			return (NULL);
		if (!philo_sleep(self))
			return (NULL);
	}
	return (NULL);
}

int main(int argc, char const *argv[])
{
	t_arg_info arg;
	size_t i;
	t_philo *philos;

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
	i = 0;
	if (arg.philo_count % 2 == 1)
	{
		while (i < arg.philo_count)
		{
			pthread_create(threads + i, NULL, philo_start, &philos[i]);
			i++;
		}
	}
	else
	{
		while (i < arg.philo_count)
		{
			pthread_create(threads + i, NULL, philo_start, &philos[i]);
			i += 2;
		}
		usleep(400);
		i = 1;
		while (i < arg.philo_count)
		{
			pthread_create(threads + i, NULL, philo_start, &philos[i]);
			i += 2;
		}
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
