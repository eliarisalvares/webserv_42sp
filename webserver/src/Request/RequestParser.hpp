/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 16:34:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/04 14:28:00 by feralves         ###   ########.fr       */
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
# define HTAB '\t'

// others
# define COLON ':'
# define POINT '.'
# define SLASH '/'

// error messages
# define HTTP_VERSION "invalid HTTP version"

# define HEADER_LIMIT_SIZE 10240 // ???????????????

typedef std::map<std::string, std::string>	t_string_map;
typedef std::pair<std::string, std::string>	t_string_pair;

class RequestParser {
public:
	RequestParser(void);
	RequestParser(Request* request);
	RequestParser(RequestParser const& copy);
	RequestParser& operator=(RequestParser const& copy);
	~RequestParser(void);

	enum Step {
		INIT,
		FIRST_LINE,
		METHOD,
		URI,
		PROTOCOL,
		VERSION,
		CR_FIRST_LINE,
		HEADER,
		HEADER_NAME,
		HEADER_VALUE_INIT,
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
		// HTAB, // (horizontal tab)
		// LF, // (line feed)
		OCTET, // (any 8-bit sequence of data)
		// SP, // (space)
		VCHAR, //(any visible US-ASCII character)
	}           t_abnf_rules;

	Step			step(void) const;
	void			setStep(Step s);
	void			method(char c);
	void			uri(char c);
	void			protocol(char c);
	void			version(char c);
	void			header(char c);
	void			body(char c);

	void			check_crlf(char c);
	void			check_request(void);

	// void			break_data(char* buffer, size_t bytes_read);

private:
	size_t								_idx;
	Step								_step;
	Request*							_request;

	// request first line
	std::string		_method;
	std::string		_uri;
	std::string		_protocol;
	std::string		_version;
	static std::string const	_right_protocol;
	void						_check_uri(void);
	void						_check_method(void);

	// headers
	int									_header_size;
	std::string							_field_name;
	std::string							_field_value;
	std::string							_last_header;
	std::map<std::string, std::vector<std::string> >	_headers;
	void								_add_header(void);
	void								_print_headers(void);
	void								_check_host(void);
	void								_check_content_length(void);
	void								_check_transfer_encoding(void);
	void								_check_post_headers(void);
	void								_parse_field_name(char c);
	void								_parse_field_value(char c);

	// body
	bool								_has_content_length;
	bool								_is_chunked;
	size_t								_content_length;
	size_t								_max_body_size;
	size_t								_body_bytes_readed;
	// size_t								_chunk_bytes_readed;
	std::vector<char>					_body;
	std::vector<char>::iterator			_body_it;
	void								_print_body(void);
	void								_body_chunked(char c);

	// throw exceptions
	void	_invalid_request(
		std::string const description,
		std::string const value,
		http::HttpStatus error_code
	);
	void	_invalid_request(
		std::string const description,
		char const c,
		http::HttpStatus error_code
	);
	void	_invalid_request(
		std::string const description,
		http::HttpStatus error_code
	);

};

#endif
