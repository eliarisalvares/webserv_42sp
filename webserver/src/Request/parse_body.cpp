/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_body.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 00:24:26 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/12 21:58:50 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

void RequestParser::body(char c) {
	if (_is_chunked)
		return _body_chunked(c);
	if (_body_bytes_readed < _content_length) {
		if (_media_type == http::MULTIPART_FORM_DATA)
			_parse_multipart(c);
		else
			_body.push_back(c);
		++_body_bytes_readed;
		if (_body_bytes_readed == _content_length)
			_step = BODY_LENGTH_END;
	}
}

void RequestParser::end_body(char c) {
	if (c == CR)
		_step = CR_BODY;
	else if (c == LF) {
		_step = END;
	}
	else
		_invalid_request("Body larger than specified", http::CONTENT_TOO_LARGE);
}

void RequestParser::_body_chunked(char c) {
	switch (_step) {
		case CHUNK_SIZE:
			_parse_chunk_size(c);
			break;
		case CHUNK_PARAMETERS:
			if (c == CR)
				_step = CR_CHUNK_SIZE;
			if (c == LF)
				_step = CHUNK_DATA;
			break;
		case CHUNK_DATA:
			_parse_chunk_data(c);
			break;
		case CHUNK_DATA_END:
			if (c == CR)
				_step = CR_CHUNK_DATA;
			else if (c == LF)
				_step = CHUNK_SIZE;
			else
				_invalid_request(
					"chunk data larger than specified", http::CONTENT_TOO_LARGE
				);
			break;
		case CHUNK_END:
			if (c == CR)
				_step = SECOND_CR_CHUNK_END;
			else if (c == LF)
				_step = END;
			else
				_invalid_request(
					"chunk data larger than specified", http::CONTENT_TOO_LARGE
				);
			break;

		default:
			break;
	}
}

void RequestParser::_parse_chunk_size(char c) {
	if (c == CR || c == LF || c == SEMICOLON) {
		if (c == CR)
			_step = CR_CHUNK_SIZE;
		else if (c == LF)
			_step = CHUNK_DATA;
		else if (c == SEMICOLON)
			_step = CHUNK_PARAMETERS;
		_chunk_size = utils::xtod(_chunk_size_str);
		_chunk_bytes_readed = 0;
		if (_chunk_size == 0) {
			if (c == CR)
				_step = CR_CHUNK_END;
			if (c == LF)
				_step = CHUNK_END;
		}
		_chunk_size_str.clear();
	}
	else if (!std::isxdigit(c))
		_bad_request("Invalid character on chunk size");
	else
		_chunk_size_str.push_back(c);
}

void RequestParser::_parse_chunk_data(char c) {
	if (_body_bytes_readed == _max_body_size)
		_invalid_request(
			"Data transfered bigger than max body size",
			http::CONTENT_TOO_LARGE
		);
	if (_chunk_bytes_readed < _chunk_size) {
		if (_media_type == http::MULTIPART_FORM_DATA)
			_parse_multipart(c);
		else
			_body.push_back(c);
		++_chunk_bytes_readed;
		++_body_bytes_readed;
		if (_chunk_bytes_readed == _chunk_size) {
			_step = CHUNK_DATA_END;
		}
	}
}

void RequestParser::_print_body(void) {
	std::vector<char>::iterator it, end = _body.end();

	if (DEBUG) {
		std::cout << GREY
				<< "------------------------------------------------------\n"
				<< BLUE << "Body of size " << _body.size()
				<< ":\n" << GREY;
		for(it = _body.begin(); it != end; ++it) {
			// if (*it == CR)
			// 	std::cout << " CR";
			// else if (*it == LF)
			// 	std::cout << "LF" << *it;
			// else
			std::cout << *it;
		}
		std::cout
				<< "\n------------------------------------------------------\n"
				<< RESET << std::endl;
	}
}

void RequestParser::parse_body(void) {
	if (_media_type == http::MULTIPART_FORM_DATA)
		_remove_boundary();
	if (_media_type == http::FORM_URLENCODED)
		_parse_form_data();
	else if (_multipart_type == http::FORM_URLENCODED)
		_parse_form_data();
	if (_media_type == http::TEXT_PLAIN || _multipart_type == http::TEXT_PLAIN) {
		_body_iterator = _body.begin();
		_body_iterator_end = _body.end();

		std::string text;
		for (_body_iterator = _body.begin(); _body_iterator != _body_iterator_end; ++_body_iterator)
			text.push_back(*_body_iterator);
		_request->setText(text);
	}
	//_print_body();
}

void RequestParser::_parse_form_data(void) {
	FormData step = FIELD_NAME;
	std::string hexadecimal;
	bool is_hexadecimal = false;

	_body_iterator_first = _body.begin();
	_body_iterator_end = _body.end();
	_body_iterator = _body_iterator_first;
	_field_name.clear();
	_field_value.clear();
	hexadecimal.clear();
	if (DEBUG)
		Logger::info("Parsing form url encoded data:");
	while (_body_iterator != _body_iterator_end) {
		switch (step)
		{
		case FIELD_NAME:
			if (*_body_iterator == EQUAL)
				step = FIELD_VALUE;
			else if (!http::is_uri_char(*_body_iterator))
				_bad_request("Field name in POST with invalid character");
			else if (is_hexadecimal) {
				if (hexadecimal.size() == 1) {
					hexadecimal.push_back(*_body_iterator);
					_field_name.push_back(utils::xtod(hexadecimal));
					is_hexadecimal = false;
					hexadecimal.clear();
				}
				else
					hexadecimal.push_back(*_body_iterator);
			}
			else if (*_body_iterator == PERCENT)
				is_hexadecimal = true;
			else
				_field_name.push_back(*_body_iterator);
			break;
		case FIELD_VALUE:
			if (*_body_iterator == AMPERSEND) {
				step = FIELD_NAME;
				_request->addPostData(_field_name, _field_value);
				_field_name.clear();
				_field_value.clear();
			}
			else if (!http::is_uri_char(*_body_iterator))
				_bad_request("Field value in POST with invalida character");
			else if (is_hexadecimal) {
				if (hexadecimal.size() == 1) {
					hexadecimal.push_back(*_body_iterator);
					_field_value.push_back(utils::xtod(hexadecimal));
					is_hexadecimal = false;
					hexadecimal.clear();
				}
				else
					hexadecimal.push_back(*_body_iterator);
			}
			else if (*_body_iterator == PERCENT)
				is_hexadecimal = true;
			else
				_field_value.push_back(*_body_iterator);
			break;

		default:
			break;
		}
		++_body_iterator;
	}
	_request->addPostData(_field_name, _field_value);
	_request->printPostData();
	_field_name.clear();
	_field_value.clear();
}

void RequestParser::_parse_multipart(char c) {
	switch (_multipart_step)
	{
	case INITIAL_BOUNDARY:
		_check_boundary_delimiter(c);
		break;
	case BOUNDARY:
		_check_boundary(c);
		break;
	case CONTENT_DISPOSITION:
		_check_content_disposition(c);
		break;
	case CONTENT_TYPE:
		_check_data_content_type(c);
		break;
	case CONTENT:
		_get_multipart_data(c);
		break;
	case CRLF_BOUNDARY:
	case CRLF_DISPOSITION:
	case CRLF_CONTENT_TYPE:
	case CRLF_INITIAL_CONTENT1:
	case CRLF_INITIAL_CONTENT2:
	case CRLF_CONTENT:
		_check_multipart_crfl(c);
		break;
	default:
		break;
	}
}

void RequestParser::_check_boundary_delimiter(char c) {
	_multipart_tmp.push_back(c);
	if (_multipart_tmp.size() == 2) {
		if (_multipart_tmp == "--") {
			_multipart_step = BOUNDARY;
			Logger::debug("Boundary delimiter OK", _multipart_tmp);
			_multipart_tmp.clear();
		}
		else {
			Logger::error("Boundary delimiter KO", _multipart_tmp);
			_bad_request("Wrong boundary delimiter");
		}
	}
}

void RequestParser::_check_boundary(char c) {
	if (c != CR) {
		_multipart_tmp.push_back(c);
		return ;
	}
	std::transform(
		_multipart_tmp.begin(),
		_multipart_tmp.end(),
		_multipart_tmp.begin(),
		&utils::c_tolower
	);
	if (_multipart_tmp != _boundary)
			_bad_request("Different boundary than specified");
	_multipart_step = CRLF_BOUNDARY;
	Logger::debug("Boundary OK", _multipart_tmp);
	_multipart_tmp.clear();
}

void RequestParser::_check_multipart_crfl(char c) {
	if (_multipart_step == CRLF_CONTENT && c != LF) {
		_body.push_back(CR);
		_body.push_back(c);
		std::cout << CR << c;
		return ;
	}
	if (_multipart_step == CRLF_INITIAL_CONTENT1) {
		if (c == CR)
			_multipart_step = CRLF_INITIAL_CONTENT2;
		else
			_bad_request("Wrong character to init content on multipart/form-data");
		return ;
	}
	if (c != LF)
		_bad_request("Wrong CRLF on multipart/form-data");
	switch (_multipart_step)
	{
	case CRLF_BOUNDARY:
		_multipart_step = CONTENT_DISPOSITION;
		break;
	case CRLF_DISPOSITION:
		_multipart_step = CONTENT_TYPE;
		break;
	case CRLF_CONTENT_TYPE:
		_multipart_step = CRLF_INITIAL_CONTENT1;
		break;
	case CRLF_INITIAL_CONTENT2:
		_multipart_step = CONTENT;
		break;
	case CRLF_CONTENT:
		_multipart_step = INITIAL_BOUNDARY;
		break;
	default:
		break;
	}
}

void RequestParser::_check_content_disposition(char c) {
	if (c != CR) {
		_multipart_tmp.push_back(c);
		return ;
	}
	Logger::debug("Content-Disposition line", _multipart_tmp);
	std::transform(
		_multipart_tmp.begin(),
		_multipart_tmp.end(),
		_multipart_tmp.begin(),
		&utils::c_tolower
	);

	std::vector<std::string> ctd = ftstring::split(_multipart_tmp, SEMICOLON);
	if (ctd.size() < 2)
		_bad_request("missing data for Content-Disposition on multipart data");
	if (ctd[0] != "content-disposition: form-data")
		_bad_request("bad Content-Disposition on multipart data");

	size_t i = 0;
	while (ctd[1][i] == SP)
		i++;
	if (ctd[1].substr(i, 5) != "name=")
		_bad_request("missing Content-Disposition name parameter");

	std::vector<std::string>::iterator it, end = ctd.end();
	std::string aux_str;
	size_t aux;

	for (it = ctd.begin() + 1; it != end; ++it) {
		aux = (*it).find(EQUAL);
		if (aux == std::string::npos)
			_bad_request("Content-Disposition parameter");

		i = 0;
		while ((*it)[i] == SP)
			i++;

		aux_str = (*it).substr(i, aux - i);
		if (aux_str == "filename")
			_request->addPostData(
				"filename", (*it).substr(aux + 2, (*it).size() - aux - 2)
			);
	}
	_request->post_data();
	_multipart_tmp.clear();
	_multipart_step = CRLF_DISPOSITION;
}

void RequestParser::_check_data_content_type(char c) {
	if (c != CR) {
		_multipart_tmp.push_back(c);
		return ;
	}
	Logger::debug("Content-Type line", _multipart_tmp);
	_multipart_step = CRLF_CONTENT_TYPE;

	if (_multipart_tmp.size() == 0) {
		_multipart_type = http::TEXT_PLAIN;
		return ;
	}

	std::transform(
		_multipart_tmp.begin(),
		_multipart_tmp.end(),
		_multipart_tmp.begin(),
		&utils::c_tolower
	);
	size_t i = _multipart_tmp.find(COLON);
	++i;
	if (_multipart_tmp.substr(0, i) != "content-type:")
		_bad_request("bad Content-Type header on multipart/form-data");
	while (_multipart_tmp[i] == SP)
		i++;
	_multipart_tmp = _multipart_tmp.substr(i, _multipart_tmp.size() - i);
	_request->setMediaType(http::str_to_enum_media_type(_multipart_tmp));
	_multipart_tmp.clear();
}

void RequestParser::_get_multipart_data(char c) {
	_body.push_back(c);
}

void RequestParser::_remove_boundary(void) {
	_body_iterator_end = _body.end();
	_body_iterator_first = _body_iterator_end - 8 - _boundary.size();
	_body.erase(_body_iterator_first, _body_iterator_end);
	_multipart_type = _request->media_type();

	if (_body.size()) {
		_request->setHasImage(true);
		_request->setImage(&_body);
		if (_multipart_type == http::IMAGE_PNG)
			_request->setImageType("png");
		else
			_request->setImageType("jpg");
	}
}
