/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_print.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbousque <tbousque@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/13 09:46:50 by tbousque          #+#    #+#             */
/*   Updated: 2022/10/13 09:50:04 by tbousque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_state(t_philo *const self)
{
	unsigned long long	time;
	const char			*state_string[6] = {"%lli %lu is eating\n",
		"%lli %lu is thinking\n", "%lli %lu is sleeping\n", "%lli %lu died\n",
		"%lli %lu has taken a fork\n", "%lli %lu has taken a fork\n"};

	pthread_mutex_lock(&self->input->can_print);
	if (self->input->simulation_stop)
		self->state = DEAD;
	else
	{
		time = get_ms(self->input->time_begin);
		if (has_died(self, time))
		{
			self->input->simulation_stop = 1;
			self->state = DEAD;
		}
		printf(state_string[self->state], time, self->index);
		if (self->state == FORK2)
		{
			self->state = EAT;
			printf(state_string[self->state], time, self->index);
		}
	}
	pthread_mutex_unlock(&self->input->can_print);
}
