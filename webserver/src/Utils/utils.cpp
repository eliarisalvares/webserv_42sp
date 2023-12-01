/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 00:44:23 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/02 12:05:36 by feralves         ###   ########.fr       */
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
		return "Invalid method";
	default:
		return "GeneralException";
	}
}

}

