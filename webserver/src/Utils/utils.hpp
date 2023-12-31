/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 00:44:35 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/12 18:56:48 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <exception>

namespace utils {

	enum Errors {
		GENERAL = 0,
		INVALID_METHOD,
		INVALID_DIRECTORY,
		UNSUPPORTED_MEDIA_TYPE,
		REQUEST_DELETED_ON_BUILDER_OR_PARSER,
	};

	class GeneralException;

	unsigned char c_tolower(unsigned char c);
	bool is_ctl(char c);
	bool is_ualpha(char c);
	bool is_lalpha(char c);
	int xtod(char c);
	size_t xtod(std::string hex);
}

class utils::GeneralException: public std::exception {
	utils::Errors _error;

	public:
		GeneralException(utils::Errors const error);
		virtual const char* what() const throw();
};

bool	checkFileWorks(const std::string& fileName);
bool	checkArgs(int argc, char *argv[]);

#endif
