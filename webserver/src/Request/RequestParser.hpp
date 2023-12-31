/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 16:34:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/12 17:06:26 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER
# define REQUEST_PARSER

# include <map>
# include <vector>
# include <string>
# include <iostream>
# include <algorithm> // transform
# include <stdio.h> // opendir, closedir

# include "Logger.hpp"
# include "http.hpp"
# include "Request.hpp"

/* Special chars - ABNF Rules */
# define CR '\r'
# define LF '\n'
# define SP ' '
# define HTAB '\t'

/* others */
# define COLON ':'
# define COMMA ','
# define POINT '.'
# define SLASH '/'
# define EQUAL '='
# define DASH '-'
# define SEMICOLON ';'
# define AMPERSEND '&'
# define PERCENT '%'

/* error messages */
# define REQPARSER_ERROR "request parser error: "
# define HTTP_VERSION "invalid HTTP version"

typedef std::map<std::string, std::string>	t_string_map;
typedef std::pair<std::string, std::string>	t_string_pair;
typedef std::map<std::string, std::vector<std::string> > t_header_map;
typedef std::map<std::string, std::vector<std::string> >::iterator t_header_iterator;

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
		BODY_LENGTH_END,
		CR_BODY,
		CHUNK_SIZE,
		CHUNK_PARAMETERS,
		CR_CHUNK_SIZE,
		CHUNK_DATA,
		CHUNK_DATA_END,
		CHUNK_END,
		CR_CHUNK_DATA,
		CR_CHUNK_END,
		SECOND_CR_CHUNK_END,
		END,
	};

	enum FormData {
		FIELD_NAME,
		FIELD_VALUE,
	};

	enum MultipartFormData {
		INITIAL_BOUNDARY,
		BOUNDARY,
		CRLF_BOUNDARY,
		CONTENT_DISPOSITION,
		CRLF_DISPOSITION,
		CONTENT_TYPE,
		CRLF_CONTENT_TYPE,
		CRLF_INITIAL_CONTENT1,
		CRLF_INITIAL_CONTENT2,
		CONTENT,
		CRLF_CONTENT,
	};

	void						init(char c);
	void						method(char c);
	void						uri(char c);
	void						protocol(char c);
	void						version(char c);
	void						header(char c);
	void						body(char c);
	void						end_body(char c);
	void						parse_body(void);

	void						check_crlf(char c);
	void						check_request(void);

	// getters
	Step						step(void) const;
	bool						has_content_length(void) const;
	bool						has_content_type(void) const;
	bool						is_chunked(void) const;
	size_t						content_length(void) const;
	size_t						max_body_size(void) const;
	size_t						body_bytes_readed(void) const;
	size_t						chunk_bytes_readed(void) const;
	size_t						chunk_size(void) const;
	Request*					getRequest(void) const;
	std::string					getMethod(void) const;
	std::string					getUri(void) const;
	std::string					getProtocol(void) const;
	std::string					getVersion(void) const;
	std::string					getFieldName(void) const;
	std::string					getFieldValue(void) const;
	std::string					getLastHeader(void) const;
	std::string					getChunkSizeStr(void) const;
	t_header_map				getHeaders(void) const;
	std::vector<char>			getBody(void) const;
	std::string					getContentType(void) const;
	std::string					getMediaTypeStr(void) const;
	std::string					getBoundary(void) const;
	http::MediaType				getMediaType(void) const;
	MultipartFormData			getMultipartStep(void) const;

	// setters
	void						setStep(Step s);

private:
	Step						_step;
	Request*					_request;

	// request first line
	std::string					_method;
	std::string					_uri;
	std::string					_protocol;
	std::string					_version;
	static std::string const	_right_protocol;
	void						_check_uri(void);
	void						_check_method(void);

	// headers
	std::string					_field_name;
	std::string					_field_value;
	std::string					_last_header;
	t_header_map				_headers;

	void						_add_header(void);
	void						_print_headers(void);
	void						_check_host(void);
	void						_check_content_length(void);
	void						_set_content_type(void);
	void						_check_transfer_encoding(void);
	void						_check_post_headers(void);
	void						_check_content_type(void);
	void						_parse_field_name(char c);
	void						_parse_field_value(char c);

	// body
	bool						_has_content_length;
	bool						_is_chunked;
	size_t						_content_length;
	size_t						_max_body_size;
	size_t						_body_bytes_readed;
	size_t						_chunk_size;
	size_t						_chunk_bytes_readed;
	std::string					_chunk_size_str;
	std::vector<char>			_body;
	std::vector<char>::iterator	_body_iterator_first;
	std::vector<char>::iterator	_body_iterator;
	std::vector<char>::iterator	_body_iterator_end;
	bool						_has_content_type;
	std::string					_content_type;
	std::string					_media_type_str;
	std::string					_boundary;
	size_t						_boundary_size;
	http::MediaType				_media_type;
	MultipartFormData			_multipart_step;
	std::string					_multipart_tmp;
	std::map<std::string, std::map<std::string, std::string> > _multipart_data;
	http::MediaType				_multipart_type;
	std::string					_multipart_name;
	int							_multipart_content_crfl;

	void						_print_body(void);
	void						_body_chunked(char c);
	void						_parse_chunk_size(char c);
	void						_parse_chunk_data(char c);
	void						_parse_form_data(void);
	void						_parse_multipart(char c);
	void						_check_boundary_delimiter(char c);
	void						_check_boundary(char c);
	void						_check_content_disposition(char c);
	void						_check_data_content_type(char c);
	void						_check_multipart_crfl(char c);
	void						_get_multipart_data(char c);
	void						_remove_boundary(void);

	// throw exceptions
	void	_bad_request(std::string const description);
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
