/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_headers.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 00:21:52 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/10 13:33:19 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

// headers
// servers should treat headers as an unordered set
// one line per header, of the form "Header-Name: value", ending with CRLF
// you should handle LF correctly
// HTTP 1.1 defines 46 headers, and one (Host:) is required in requests.

// A server MUST NOT apply a request to the target resource until it receives the
// entire request header section, since later header field lines might include
// conditionals, authentication credentials, or deliberately misleading duplicate
// header fields that could impact request processing
void RequestParser::header(char c) {
	if (_step == HEADER) {
		if (c == CR) {
			_step = SECOND_CR_HEADER;
			return ;
		}
		else if (c == LF) {
			Logger::debug("LF end header");
			_step = END;
			return ;
		}
		else if (c == SP || c == HTAB) {
			_field_name = _last_header;
			_step = HEADER_VALUE_INIT;
		}
		else
			_step = HEADER_NAME;
	}
	switch (_step) {
		case HEADER_NAME:
			_parse_field_name(c);
			break;
		case HEADER_VALUE_INIT:
			if (c == SP || c == HTAB)
				return ;
			_step = HEADER_VALUE;
			_parse_field_value(c);
			break;
		case HEADER_VALUE:
			_parse_field_value(c);
			break;
		default:
			break;
	}
}

// Field name:
// The requested field name. It MUST conform to the field-name syntax defined
// in Section 5.1, and it SHOULD be restricted to just letters, digits,
// and hyphen ('-') characters, with the first character being a letter.
// nginx não segue essas regras, aceita qualquer coisa, google tb...

// The header name is not case-sensitive (though the value may be) - Header
// name can be either title-case or lowercase or mixed, all are valid

// nos headers parece considerar um espaço no começo do nome do header como parte dele
// interpreta " Host" e não "Host" (dá erro porque considera que o header não está incluso)
// Header lines beginning with space or tab are actually part of the previous header line, folded into multiple lines for easy reading.
// the following two headers are equivalent:

// Header1: some-long-value-1a, some-long-value-1b
// HEADER1:    some-long-value-1a,
//             some-long-value-1b

void RequestParser::_parse_field_name(char c) {
	if (c != COLON && !utils::is_ctl(c) && c != SP)
		_field_name.push_back(c);
	else if (c == COLON) {
		Logger::debug("key", _field_name);
		_step = HEADER_VALUE_INIT;
	}
	else if (c == CR) {
		// Logger::warning("field name without value: " + _field_name);
		_step = CR_HEADER;
		_add_header();
	}
	else
		_invalid_request("field name character", c, http::BAD_REQUEST);
}

// Any number of spaces or tabs may be between the ":" and the value
void RequestParser::_parse_field_value(char c) {
	// if (c == SP || c == HTAB) // só pode ser no começo
	// 	return ;
	if (c == CR || c == LF) {
		Logger::debug("value", _field_value);
		if (c == CR)
			_step = CR_HEADER;
		else if (c == LF) {
			Logger::debug("LF header");
			_step = HEADER;
		}
		_add_header();
	}
	else if (utils::is_ctl(c) && c != HTAB)
		_bad_request("request parser: control character on field value");
	else
		_field_value.push_back(c);
}

void RequestParser::_add_header(void) {
	t_header_iterator it;

	std::transform(
		_field_name.begin(),
		_field_name.end(),
		_field_name.begin(),
		&utils::c_tolower
	);
	_last_header = _field_name; // verificar se faz deep copy...
	it = _headers.find(_field_name);
	if (it == _headers.end()) {
		// não tem o header
		std::vector<std::string> v;

		v.clear();
		v.push_back(_field_value);
		_headers.insert(std::make_pair(_field_name, v));
	}
	else // tem o header - se a linha começou com espaço/tab; se não dá erro!!
		it->second.push_back(_field_value); // assumindo que ele já estará limpo de espaços e tabs no início e fim
	_field_value.clear();
	_field_name.clear();
}
// remove any leading and trailing HTTP whitespace bytes from potentialValue

// header Host is mandatory and singleton
void RequestParser::_check_host(void) {
	t_header_iterator it;

	it = _headers.find("host");
	if (it == _headers.end())
		_bad_request("'Host' header not found");
	if (it->second.size() > 1 || it->second[0].size() == 0)
		_bad_request("'Host' header");
	_request->setHost(it->second[0]);
}

// header Content-Length has only numbers, is singleton and has a maximum size
void RequestParser::_check_content_length(void) {
	t_header_iterator it_header;

	it_header = _headers.find("content-length");
	if (it_header == _headers.end())
		return ;
	if (it_header->second.size() > 1)
		_bad_request("'Content-Length' header");
	_has_content_length = true;

	std::string content_lenght = it_header->second[0];
	std::string::iterator it, end = content_lenght.end();
	for (it = content_lenght.begin(); it != end; ++it) {
		if (!std::isdigit(*it))
			_invalid_request(
				"'Content-Length' header invalid character",
				*it,
				http::BAD_REQUEST
			);
	}

	std::stringstream ss(content_lenght);
	ss >> _content_length;
	if (_content_length > _max_body_size)
		_invalid_request(
			"Content-Lenght bigger than max body size",
			http::CONTENT_TOO_LARGE
		);
	_request->setContentLength(_content_length);
}

void RequestParser::_check_transfer_encoding(void) {
	t_header_iterator it_header;

	it_header = _headers.find("transfer-encoding");
	if (it_header == _headers.end())
		return ;
	if (it_header->second.size() > 1)
		_bad_request("'Transfer-Encoding' header");
	if (it_header->second[0].compare("chunked") != 0)
		_invalid_request("Transfer-Encoding type", http::NOT_IMPLEMENTED);
	_is_chunked = true;
	Logger::debug("found Transfer-Encoding: chunked");
}

// content-type is singleton and the last value passed is its final value
// (if it is not an empty value)
void RequestParser::_set_content_type(void) {
	t_header_iterator it_header;
	std::string ct_raw;
	std::vector<std::string> ct_v;
	std::vector<std::string>::iterator it, end;

	it_header = _headers.find("content-type");
	if (it_header == _headers.end())
		return ;

	ct_v = it_header->second;
	end = ct_v.end();
	for (it = ct_v.begin(); it != end; ++it) {
		if ((*it).size() > 0)
			_content_type = *it;
	}

	if (_content_type.find(',') != std::string::npos) {
		ct_v = ftstring::split(_content_type, ',');
		end = ct_v.end();
		for (it = ct_v.begin(); it != end; ++it) {
			if ((*it).size() > 0)
				_content_type = *it;
		}
	}

	if (_content_type.size())
		_has_content_type = true;
}

void RequestParser::_check_post_headers(void) {
	if (!_is_chunked && (!_has_content_length || !_content_length))
		_invalid_request(
			"POST without 'Transfer-Encoding' or 'Content-Length' headers",
			http::LENGTH_REQUIRED
		);
	if (_is_chunked && _has_content_length && _content_length)
		_bad_request(
			"chunked data and content-length both setted"
		);
	if (_content_length)
		_body.reserve(_content_length);
	_step = BODY;
	if (_is_chunked)
		_step = CHUNK_SIZE;
	if (!_has_content_type)
		_bad_request("missing Content-Type");
	_check_content_type();
}

void RequestParser::_check_content_type(void) {
	std::set<std::string>::const_iterator it;
	std::string::iterator it_str, end_str;

	Logger::debug("_content_type", _content_type);
	end_str = _content_type.end();
	for (it_str = _content_type.begin(); it_str != end_str; ++it_str)
		*it_str = utils::c_tolower(*it_str);

	std::vector<std::string> ct_split = ftstring::split(
		_content_type, SEMICOLON
	);
	_media_type_str = ct_split[0];
	Logger::debug("_media_type_str", _media_type_str);
	_media_type = http::str_to_enum_media_type(_media_type_str);

	if (_media_type == http::MULTIPART_FORM_DATA) {
		if (ct_split.size() < 2)
			_bad_request(
				"Missing boundary info for Content-Type = multipart/form-data"
			);
		std::vector<std::string>::iterator it_v, end_v = ct_split.end();
		for (it_v = ct_split.begin() + 1; it_v != end_v; ++it_v) {
			Logger::debug("ct_split", *it_v);
			std::vector<std::string> bdry = ftstring::split(*it_v, EQUAL);
			if (bdry.size() == 2) {
				size_t i = 0;
				while (i < bdry[0].size() && bdry[0][i] == SP)
					++i;
				if (i > 0)
					bdry[0].erase(0, i);
				if(bdry[0] == "boundary") {
					_boundary = bdry[1];
					_boundary_size = _boundary.size();
					Logger::warning("Found boundary", _boundary);
				}
			}
		}
		if (_boundary.size() == 0)
			_bad_request(
				"Boundary info for Content-Type = multipart/form-data is empty"
			);
	}
	_request->setMediaType(_media_type);
}

void RequestParser::_print_headers(void) {
	if (DEBUG) {
		t_header_iterator it, end;
		std::vector<std::string>::iterator v_it, v_end;
		int i;

		end = _headers.end();
		std::cout << GREY
				<< "------------------------------------------------------\n"
				<< BLUE << "Headers saved:\n" << GREY;
		for (it = _headers.begin(); it != end; ++it) {
			i = 0;
			std::cout << it->first << ": ";
			v_end = it->second.end();
			for (v_it = it->second.begin(); v_it != v_end; ++v_it) {
				++i;
				if (i == 1)
					std::cout << *v_it;
				else
					std::cout << "," << *v_it;
			}
			std::cout << std::endl;
		}
		std::cout << "------------------------------------------------------\n"
				<< RESET;
	}
}



// Representation headers include: Content-Type, Content-Encoding, Content-Language, and Content-Location.

// Content-Type (ignora os parametros adicionais)
// application/x-www-form-urlencoded, multipart/form-data, or text/plain, image/...
// MIME type: type/subtype;parameter=value


// application/octet-stream -> arquivo binário -> vou recusar
// na vdd vou recusar tipos diferentes de imagem, application/x-www-form-urlencoded, multipart/form-data, or text/...
// Aceitar:
// application/x-www-form-urlencoded, multipart/form-data
// text/plain
// text/html
// text/css
// image/gif: Graphics Interchange Format (GIF)
// image/jpeg: Joint Photographic Expert Group image (JPEG)
// image/png: Portable Network Graphics (PNG)

// application/json -> tem parse específico... (provavelmente não vou ter tempo)

// - definir o type do arquivo que salva

// 403 Forbidden
// The 403 (Forbidden) status code indicates that the server understood the request but refuses to fulfill it. A server that wishes to make public why the request has been forbidden can describe that reason in the response content (if any).

// 408 Request Timeout
// The 408 (Request Timeout) status code indicates that the server did not receive a complete request message within the time that it was prepared to wait.


// 414 URI Too Long
// The 414 (URI Too Long) status code indicates that the server is refusing to service the request because the target URI is longer than the server is willing to interpret.


//  Unsupported Media Type
// The 415 (Unsupported Media Type) status code indicates that the origin server is refusing to service the request because the content is in a format not supported by this method on the target resource.

// The format problem might be due to the request's indicated Content-Type or Content-Encoding, or as a result of inspecting the data directly.

// If the problem was caused by an unsupported content coding, the Accept-Encoding response header field (Section 12.5.3) ought to be used to indicate which (if any) content codings would have been accepted in the request.

// On the other hand, if the cause was an unsupported media type, the Accept response header field (Section 12.5.1) can be used to indicate which media types would have been accepted in the request.

// 15.5.21. 422 Unprocessable Content
// The 422 (Unprocessable Content) status code indicates that the server understands the content type of the request content (hence a 415 (Unsupported Media Type) status code is inappropriate), and the syntax of the request content is correct, but it was unable to process the contained instructions. For example, this status code can be sent if an XML request content contains well-formed (i.e., syntactically correct), but semantically erroneous XML instructions.

// check de content-type - se POST apenas
// - verifica se é um tipo aceito
// - pega parametro se tiver (deppis do ;)
