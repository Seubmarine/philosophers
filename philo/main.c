#define __USE_XOPEN_EXTENDED 1
// #define __USE_MISC 1
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>

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

unsigned long long get_ms(struct timeval begin)
{
	struct timeval current;

	gettimeofday(&current, NULL);
	return ((current.tv_sec - begin.tv_sec) * 1000 + (current.tv_usec - begin.tv_usec) / 1000);
}

unsigned long long to_ms(struct timeval current)
{
	return ((current.tv_sec) * 1000 + (current.tv_usec) / 1000);
}

// unsigned long long to_usec(struct timeval current)
// {
// 	// return ((current.tv_sec) * 1000 + (current.tv_usec) / 1000);
// }

struct timeval time_passed(struct timeval begin, struct timeval current)
{
	struct timeval diff = {.tv_sec = current.tv_sec - begin.tv_sec, .tv_usec = current.tv_usec - begin.tv_usec};
	return (diff);
};

void usleep_opti(unsigned int usec, struct timeval begin)
{
	(void) usec;
	const unsigned long long time_to_reach = get_ms(begin) + usec;
	unsigned long long current;
	while (1)
	{
		current = get_ms(begin);
		// printf("to reach %llu, current: %llu\n", time_to_reach, current);
		if (current >= time_to_reach)
			return ;
		usleep(10);
	}
}

// int main(int argc, char const *argv[])
// {
// 	(void) argc;
// 	(void) argv;
// 	struct timeval begin;
// 	struct timeval current;
// 	int i = 0;
// 	while (i < 100)
// 	{
// 		gettimeofday(&begin, NULL);
// 		printf("begin = %llu\n", to_ms(begin));
// 		usleep_opti(200, begin);
// 		gettimeofday(&current, NULL);
// 		printf("current = %llu\n", to_ms(current));
// 		struct timeval diff = time_passed(begin, current);
// 		printf("diff = %llu\n", to_ms(diff));
// 		i++;
// 	}
// 	return (0);
// }

int has_died(t_philo *const self, unsigned long long current_time)
{
	return (!(current_time <= self->last_eaten + self->input->time_to_die));
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
	while (1)
	{
		current_time = get_ms(self->input->time_begin);
		if (has_died(self, current_time))
		{
			print_state(self);
			return (0);
		}	
		else if (pthread_mutex_trylock(current_fork) == 0)
		{
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
		usleep(15);
	}
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
	usleep_opti(self->input->time_to_eat, self->input->time_begin);
	pthread_mutex_unlock(self->fork_right);
	pthread_mutex_unlock(&self->fork_left);
	return (1);
}

unsigned long long time_to_do(t_philo *const self, unsigned long long current_time, unsigned long long time_to_do)
{
	(void) self;
	(void) current_time;
	if (current_time + time_to_do > self->last_eaten + self->input->time_to_die)
	{
		// self->state = DEAD;
		// return (0);
		return (self->last_eaten + self->input->time_to_die - current_time);
	}
	return (time_to_do);
}

int philo_sleep(t_philo *const self)
{
	self->state = SLEEP;
	print_state(self);
	if (self->state == DEAD)
		return (0);
	// int time_to_do_v = time_to_do(self, get_ms(self->input->time_begin), self->input->time_to_sleep);
	usleep_opti(self->input->time_to_sleep, self->input->time_begin);
	// if (time_to_do_v != self->input->time_to_sleep)
	// {
	// 	self->state = DEAD;
	// 	print_state(self);
	// 	return (0);
	// }
	return (1);
}

void *philo_start(void *arg)
{
	t_philo *const self = arg;
	// if (!(self->index % 2))
	// 	sleep(10);
	while (1)
	{
		self->state = THINK;
		print_state(self);
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
	arg = get_arg_info(argv);
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
	while (i < arg.philo_count)
	{
		pthread_create(threads + i, NULL, philo_start, &philos[i]);
		i += 2;
	}
	// usleep(1000);
	i = 1;
	while (i < arg.philo_count)
	{
		pthread_create(threads + i, NULL, philo_start, &philos[i]);
		i += 2;
	}
	// usleep_opti(arg.time_to_eat / 2, arg.time_begin);
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
