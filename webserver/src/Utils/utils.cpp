/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 00:44:23 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/07 13:38:31 by sguilher         ###   ########.fr       */
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
	case INVALID_DIRECTORY:
		return "Directory does not exist";
	case MISSING_REQUEST_ON_PARSER:
		return "Object Request is NULL on RequestParser";
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

int xtod(char c) {
  switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return (c - '0');
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
      return (c - 'a' + 10);
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
      return (c - 'A' + 10);
    default:
      return -1;
  }
}

size_t xtod(std::string hex) {
	std::string::iterator it, end = hex.end();
	size_t decimal = 0;

	for (it = hex.begin(); it != end; ++it)
		decimal = decimal * 16 + size_t(xtod(*it));
	return decimal;
}

}
