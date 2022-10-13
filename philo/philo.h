/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbousque <tbousque@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 22:43:59 by tbousque          #+#    #+#             */
/*   Updated: 2022/10/13 10:39:45 by tbousque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H
# define __USE_XOPEN_EXTENDED 1
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <stdio.h>
# include <sys/time.h>

typedef struct s_arg_info
{
	size_t			philo_count;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	unsigned long	cycle_count;
	pthread_mutex_t	can_print;
	int				simulation_stop;
	struct timeval	time_begin;
}	t_arg_info;

typedef enum e_philo_state
{
	EAT,
	THINK,
	SLEEP,
	DEAD,
	FORK1,
	FORK2
}	t_philo_state;

typedef struct s_philo
{
	size_t				index;
	pthread_mutex_t		fork_left;
	pthread_mutex_t		*fork_right;
	t_philo_state		state;
	t_arg_info			*input;
	unsigned long long	last_eaten;
	unsigned long long	cycle_current;
}	t_philo;

//time helper

unsigned long long	get_ms(struct timeval begin);
unsigned long long	time_to_do(t_philo *const self, \
	unsigned long long current_time, unsigned long long time_to_do);
int					has_died(t_philo *const self, \
	unsigned long long current_time);
size_t				philos_pthread_create(t_philo *philos, \
	pthread_t *threads, size_t philo_count, void *(*philo_routine)(void *));
void				*philo_start(void *arg);
void				print_state(t_philo *const self);

//0 on success, 1 on error
int					get_arg_info(int argc, char const *argv[], t_arg_info *arg);
#endif