/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbousque <tbousque@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/13 09:43:53 by tbousque          #+#    #+#             */
/*   Updated: 2022/10/13 09:49:55 by tbousque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	philo_has_one_fork(t_philo *const self, pthread_mutex_t *current_fork, \
	int fork_pos)
{
	unsigned long long	current_time;
	const t_philo_state	previous_state = self->state;

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

int	philo_has_two_fork_in_time(t_philo *const self, pthread_mutex_t *a, \
	pthread_mutex_t *b)
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

int	philo_eat(t_philo *const self)
{
	self->state = DEAD;
	if (self->index % 2)
	{
		if (philo_has_two_fork_in_time(self, &self->fork_left, \
			self->fork_right))
			self->state = EAT;
	}
	else
	{
		if (philo_has_two_fork_in_time(self, self->fork_right, \
			&self->fork_left))
			self->state = EAT;
	}
	if (self->state == DEAD)
		return (0);
	self->last_eaten = get_ms(self->input->time_begin);
	usleep(time_to_do(self, get_ms(self->input->time_begin), \
		self->input->time_to_eat) * 1000);
	pthread_mutex_unlock(self->fork_right);
	pthread_mutex_unlock(&self->fork_left);
	return (1);
}

int	philo_sleep(t_philo *const self)
{
	self->state = SLEEP;
	print_state(self);
	if (self->state == DEAD)
		return (0);
	usleep(time_to_do(self, \
		get_ms(self->input->time_begin), self->input->time_to_sleep) * 1000);
	if (has_died(self, get_ms(self->input->time_begin)))
	{
		self->state = DEAD;
		print_state(self);
		return (0);
	}
	return (1);
}

void	*philo_start(void *arg)
{
	t_philo *const	self = arg;

	while (1)
	{
		if (self->input->cycle_count)
			if (self->input->cycle_count <= self->cycle_current)
				break ;
		self->state = THINK;
		print_state(self);
		if (self->input->philo_count % 2 == 1 && self->cycle_current)
			usleep(self->input->time_to_eat * 1000);
		self->cycle_current++;
		if (self->state == DEAD)
			return (NULL);
		if (!philo_eat(self))
			return (NULL);
		if (!philo_sleep(self))
			return (NULL);
	}
	return (NULL);
}
