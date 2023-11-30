/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 16:34:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/30 02:17:33 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER
# define REQUEST_PARSER

# include <map>
# include <vector>
# include <string>
# include <iostream>

# include "Logger.hpp"
# include "http.hpp"
# include "Request.hpp"

// Special chars - ABNF Rules
# define CR '\r'
# define LF '\n'
# define SP ' '

// others
# define COLON ':'

typedef std::map<std::string, std::string>	t_string_map;
typedef std::pair<std::string, std::string>	t_string_pair;

class RequestParser {
public:
	RequestParser(void);
	RequestParser(Request* request);
	RequestParser(RequestParser const& copy);
	RequestParser& operator=(RequestParser const& copy);
	~RequestParser(void);

	typedef enum e_steps {
		INIT,
		FIRST_LINE,
		METHOD,
		URI,
		PROTOCOL,
		VERSION,
		HEADER,
		END_HEADER,
		BODY,
		END_BODY,  // necessary?
		END,
		ERROR,
	}			t_steps;

	typedef enum e_parser_error {
		NONE,
		LF_WITHOUT_CR,
		CR_WITHOUT_LF,
		INVALID_METHOD_TOKEN,
		INVALID_URI,
		INVALID_PROTOCOL,
		INVALID_HTTP_VERSION,
	}           t_parser_error;

	typedef enum e_abnf_rules {
		ALPHA, // (letters)
		// CR, // (carriage return)
		CRLF, // (CR LF)
		CTL, // (controls)
		DIGIT, // (decimal 0-9)
		DQUOTE, // (double quote)
		HEXDIG, // (hexadecimal 0-9/A-F/a-f)
		HTAB, // (horizontal tab)
		// LF, // (line feed)
		OCTET, // (any 8-bit sequence of data)
		// SP, // (space)
		VCHAR, //(any visible US-ASCII character)
	}           t_abnf_rules;

	void			break_data(char* buffer, size_t bytes_read);
	void			first_line(void);
	void			method(char c);
	void			uri(char c);
	void			protocol(char c);
	void			version(char c);
	void			header(void);
	e_steps			step(void);
	e_parser_error	error(void);

	bool			first_line_not_parsed(void);
	t_string_map	get_result(void) const;

private:
	Logger								log;
	size_t								_idx;
	t_steps								_step;
	t_parser_error						_error;
	Request*							_request;
	std::string							_error_str;
	std::vector<char>					_data;
	std::vector<char>::iterator			_data_it;
	t_string_map						_result;

	std::string		_method;
	std::string		_uri;
	std::string		_protocol;
	std::string		_version;

	bool								_found_EOL(void);

	void								_print_data(void);

};

#endif
