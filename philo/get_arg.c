/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_arg.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbousque <tbousque@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/13 10:39:11 by tbousque          #+#    #+#             */
/*   Updated: 2022/10/13 10:45:36 by tbousque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_isspace(int c)
{
	return (c == ' ' || c == '\f' || \
		c == '\n' || c == '\r' || c == '\t' || c == '\v');
}

int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

int	ft_atoi(const char *nptr)
{
	int	number;
	int	sign;

	sign = 1;
	while (ft_isspace(*nptr))
		nptr++;
	if (*nptr == '-')
	{
		sign = -1;
		nptr++;
	}
	else if (*nptr == '+')
		nptr++;
	number = 0;
	while (*nptr && ft_isdigit(*nptr))
		number = (number * 10) + (*nptr++ - 48);
	number *= sign;
	if (sign == 1 && number < 0)
		return (-1);
	else if (sign == -1 && number > 0)
		return (0);
	return (number);
}

//0 on success, 1 on error
int	get_arg_info(int argc, char const *argv[], t_arg_info *arg)
{
	arg->philo_count = ft_atoi(argv[1]);
	arg->time_to_die = ft_atoi(argv[2]);
	arg->time_to_eat = ft_atoi(argv[3]);
	arg->time_to_sleep = ft_atoi(argv[4]);
	arg->simulation_stop = 0;
	arg->cycle_count = 0;
	gettimeofday(&arg->time_begin, NULL);
	if (argc == 6)
	{
		arg->cycle_count = ft_atoi(argv[5]);
		if (ft_atoi(argv[5]) <= 0)
			return (1);
	}
	if (pthread_mutex_init(&arg->can_print, NULL) != 0)
	{
		printf("Mutext \"can_print\" can't init\n");
		return (1);
	}
	if (ft_atoi(argv[1]) <= 0 || arg->time_to_eat < 0 || arg->time_to_die < 0 \
		|| arg->time_to_sleep < 0)
		return (1);
	return (0);
}
