/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 16:34:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/02 12:00:30 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER
# define REQUEST_PARSER

# include <map>
# include <vector>
# include <string>
# include <iostream>
# include <algorithm> // transform

# include "Logger.hpp"
# include "http.hpp"
# include "Request.hpp"

// Special chars - ABNF Rules
# define CR '\r'
# define LF '\n'
# define SP ' '
# define TAB '\t'

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

	enum Steps {
		INIT,
		FIRST_LINE,
		METHOD,
		URI,
		PROTOCOL,
		VERSION,
		CR_FIRST_LINE,
		HEADER,
		HEADER_NAME,
		HEADER_VALUE,
		CR_HEADER,
		SECOND_CR_HEADER,
		BODY,
		BODY_NEW_LINE,
		CR_BODY,
		SECOND_CR_BODY,
		END,
		ERROR, // necessary?
	};

	enum Error {
		NONE,
		LF_WITHOUT_CR,
		CR_WITHOUT_LF,
		INVALID_METHOD_TOKEN,
		INVALID_URI,
		INVALID_PROTOCOL,
		INVALID_HTTP_VERSION,
	};

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

	Steps			step(void);
	void			method(char c);
	void			uri(char c);
	void			protocol(char c);
	void			version(char c);
	void			header(char c);
	void			body(char c);

	void			check_crlf(char c);
	void			check_first_line(void);
	void			check_headers(void);

	// void			break_data(char* buffer, size_t bytes_read);

	// t_string_map	get_result(void) const;

private:
	Logger								log;
	size_t								_idx;
	Steps								_step;
	Request*							_request;
	// t_string_map						_result;

	// request first line
	std::string		_method;
	std::string		_uri;
	std::string		_protocol;
	std::string		_version;

	static std::string const	_right_protocol;
	// static int const			_right_version;

	// headers
	std::string							_field_name;
	std::string							_field_value;
	std::string							_last_header;
	std::map<std::string, std::vector<std::string> >	_headers;
	void								_add_header(void);
	void								_print_headers(void);
	void								_parse_field_name(char c);
	void								_parse_field_value(char c);

	// body
	std::vector<char>					_body;
	std::vector<char>::iterator			_body_it;
	void								_print_body(void);

};

#endif
