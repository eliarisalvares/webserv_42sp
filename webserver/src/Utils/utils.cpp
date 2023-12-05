/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 00:44:23 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/04 14:13:21 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"


namespace utils {

GeneralException::GeneralException(Errors const error) {
	_error = error;
}

const char* GeneralException::what() const throw() {
	switch (_error)
	{
	case INVALID_METHOD:
		return "Method not present in allowed methods list";
	default:
		return "GeneralException";
	}
}

}

void	clear_fds(void)
{
	int	fd;

	fd = 100;
	if (fd < 0)
		return ;
	while (fd > STDERR_FILENO)
		close(fd--);
}
