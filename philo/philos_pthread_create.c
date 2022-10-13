/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philos_pthread_create.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbousque <tbousque@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/13 09:09:59 by tbousque          #+#    #+#             */
/*   Updated: 2022/10/13 09:49:29 by tbousque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

//0 on success 1 on error
size_t	philos_pthread_create_odd(t_philo *philos, pthread_t *threads, \
	size_t philo_count, void *(*philo_routine)(void *))
{
	size_t	i;

	i = 0;
	if (philo_count % 2 == 1)
	{
		while (i < philo_count)
		{
			if (pthread_create(threads + i, NULL, philo_routine, &philos[i]) \
				!= 0)
				return (i);
			i++;
		}
	}
	return (0);
}

size_t	philos_pthread_create_even(t_philo *philos, pthread_t *threads, \
	size_t philo_count, void *(*philo_routine)(void *))
{
	size_t	i;

	i = 0;
	while (i < philo_count)
	{
		if (pthread_create(threads + i, NULL, philo_routine, &philos[i]) != 0)
			return (i);
		i += 2;
	}
	usleep(400);
	i = 1;
	while (i < philo_count)
	{
		if (pthread_create(threads + i, NULL, philo_routine, &philos[i]) != 0)
			return (i);
		i += 2;
	}
	return (0);
}

size_t	philos_pthread_create(t_philo *philos, pthread_t *threads, \
	size_t philo_count, void *(*philo_routine)(void *))
{
	if (philo_count % 2 == 1)
		return (philos_pthread_create_odd(philos, threads, \
			philo_count, philo_routine));
	else
		return (philos_pthread_create_even(philos, threads, \
			philo_count, philo_routine));
}
