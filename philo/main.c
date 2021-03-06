/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gborne <gborne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/06 23:23:16 by gborne            #+#    #+#             */
/*   Updated: 2022/07/14 14:32:07 by gborne           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	start_thread(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->nb_philo)
	{
		if (pthread_create(&data->philos[i].thread, NULL,
				&routine, (void*)&data->philos[i]) != 0)
			ft_error("Error\nCan't create thread.\n");
		usleep(100);
	}
}

static int	end(t_data *data)
{
	int	i;
	
	i = 0;
	while (i <= data->nb_philo)
	{
		pthread_mutex_destroy(&data->philos[i].fork);
		pthread_mutex_destroy(&data->philos[i].eat);
		i++;
	}
	free(data->philos);
	return (0);
}

static int	run(t_data *data)
{
	int					i;

	i = -1;
	while (++i < data->nb_philo)
	{
		pthread_mutex_lock(&data->philos[i].eat);
		if (data->philos[i].time + data->time_die < gettime()
		&& (data->philos[i].state != STATE_END))
		{
			pthread_mutex_lock(&data->write);
			end(data);
			pthread_mutex_unlock(&data->write);
			usleep(3000);
			msg(data, i, "died", gettime() - data->starttime - 3);
			pthread_mutex_destroy(&data->write);
			return (0);
		}
		pthread_mutex_unlock(&data->philos[i].eat);
	}
	if (data->philos_end == data->nb_philo)
	{
		pthread_mutex_lock(&data->write);
		end(data);
		pthread_mutex_unlock(&data->write);
		pthread_mutex_destroy(&data->write);
		return (0);
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_data				data;
	unsigned long long	starttime;

	starttime = gettime();
	if ((argc != 5 && argc != 6) || init(argc, argv, &data, starttime))
		return (write(1, "Error\nWrong arguments\n", 23));
	start_thread(&data);
	while (run(&data) == 1)
		;
	return (0);
}
