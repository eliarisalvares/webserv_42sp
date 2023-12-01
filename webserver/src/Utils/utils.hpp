/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 00:44:35 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/02 12:06:21 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>

namespace utils {

	enum Errors {
		GENERAL = 0,
		INVALID_METHOD,
	};

	class GeneralException;

}

class utils::GeneralException: public std::exception {
	utils::Errors _error;

	public:
		GeneralException(utils::Errors const error);
		virtual const char* what() const throw();
};

#endif
