/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_helper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbousque <tbousque@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 22:53:59 by tbousque          #+#    #+#             */
/*   Updated: 2022/10/13 00:26:46 by tbousque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

unsigned long long	get_ms(struct timeval begin)
{
	struct timeval	current;

	gettimeofday(&current, NULL);
	return ((current.tv_sec - begin.tv_sec) * 1000 + \
		(current.tv_usec - begin.tv_usec) / 1000);
}

unsigned long long	time_to_do(t_philo *const self, \
	unsigned long long current_time, unsigned long long time_to_do)
{
	if (current_time + time_to_do <= self->last_eaten + \
		self->input->time_to_die)
		return (time_to_do);
	return (self->last_eaten + self->input->time_to_die - current_time);
}

int	has_died(t_philo *const self, unsigned long long current_time)
{
	return (current_time >= self->last_eaten + self->input->time_to_die);
}
