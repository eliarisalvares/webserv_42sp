/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 00:44:23 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/04 23:04:12 by sguilher         ###   ########.fr       */
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

unsigned char c_tolower(unsigned char c) {
	return std::tolower(c);
}

bool is_ctl(char c) {
  return ((c >= 0 && c < 32) || c == 127);
}

bool is_ualpha(char c) {
  return (c >= 'A' && c <= 'Z');
}

bool is_lalpha(char c) {
  return (c >= 'a' && c <= 'z');
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
