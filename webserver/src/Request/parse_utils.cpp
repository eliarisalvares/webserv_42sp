/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 00:33:47 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/08 00:34:17 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

void RequestParser::_bad_request(std::string const description) {
	http::bad_request(REQPARSER_ERROR + description);
}

void  RequestParser::_invalid_request(
	std::string const description,
	std::string const value,
	http::HttpStatus error_code
) {
	Logger::warning(REQPARSER_ERROR + description, value);
	throw http::InvalidRequest(error_code);
}

void  RequestParser::_invalid_request(
	std::string const description,
	char const c,
	http::HttpStatus error_code
) {
	Logger::warning_no_lf(REQPARSER_ERROR + description);
	std::cout << GREY << c << std::endl;
	throw http::InvalidRequest(error_code);
}

void  RequestParser::_invalid_request(
	std::string const description,
	http::HttpStatus error_code
) {
	Logger::warning(REQPARSER_ERROR + description);
	throw http::InvalidRequest(error_code);
}

