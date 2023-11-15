/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 16:34:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/15 14:15:27 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER
# define REQUEST_PARSER

# include <map>
# include <vector>
# include <string>
# include <iostream>

# include "Logger.hpp"

// ABNF Rules

// Special chars
# define CR '\r'
# define LF '\n'
# define SP ' '

class RequestParser {
public:
	RequestParser(void);
	RequestParser(RequestParser const& copy);
	RequestParser& operator=(RequestParser const& copy);
	~RequestParser(void);

	typedef enum e_steps {
		ERROR,
		INIT,
		CRLF,
		FIRST_LINE,  // CRLF?
		METHOD,
		URI,
		PROTOCOL,
		VERSION,
		HEADER,
		END_HEADER,
		BODY,
		END_BODY,  // necessary?
		END,
	}           t_steps;

	typedef enum e_parser_error {
		NONE,
		LF_WITHOUT_CR,
		CR_WITHOUT_LF,
		INVALID_METHOD,
		INVALID_URI,
		INVALID_PROTOCOL,
		INVALID_HTTP_VERSION,
	}           t_parser_error;

	void			break_data(char* buffer, size_t bytes_read);
	void			first_line(void);
	e_steps			step(void);
	e_parser_error	error(void);

private:
	bool								_found_EOL(void);

	Logger								log;
	size_t								_idx;
	t_steps								_step;
	t_parser_error						_error;
	std::string							_error_str;
	std::vector<char>					_data;
	std::vector<char>::iterator			_data_it;
	std::map<std::string, std::string>	_result;
};

#endif
