/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 00:44:23 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/02 14:47:36 by sguilher         ###   ########.fr       */
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

