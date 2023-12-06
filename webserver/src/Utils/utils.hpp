/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 00:44:35 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/06 15:40:36 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <unistd.h> // close()

namespace utils {

	enum Errors {
		GENERAL = 0,
		INVALID_METHOD,
	};

	class GeneralException;

	unsigned char c_tolower(unsigned char c);
	bool is_ctl(char c);
	bool is_ualpha(char c);
	bool is_lalpha(char c);

}

class utils::GeneralException: public std::exception {
	utils::Errors _error;

	public:
		GeneralException(utils::Errors const error);
		virtual const char* what() const throw();
};

bool	checkFileWorks(const std::string& fileName);
bool	checkArgs(int argc, char *argv[]);
void	clear_fds(void);

#endif
