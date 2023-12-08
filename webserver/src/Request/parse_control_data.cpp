/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_control_data.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 00:28:10 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/08 11:06:12 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

// general:
// Parts of request line is separated by a space character.
// Technically there should be only one space though I've seen badly malformed requests that send multiple spaces.
// Browsers will never send more than one space
// nginx aceita vários espaços e nós tb estamos aceitando
// não aceita espaço no início da primeira linha (não é letra maiúscula)

// The three fields in the initial message line should be separated by a single space, but might instead use several spaces, or tabs. Accept any number of spaces or tabs between these fields.

// Method names are always uppercase
// The method token is case-sensitive
// When a request method is received that is unrecognized or not implemented by an origin
// server, the origin server SHOULD respond with the 501 (Not Implemented) status code.
// When a request method is received that is known by an origin
// server but not allowed for the target resource, the origin server
// SHOULD respond with the 405 (Method Not Allowed) status code.
void RequestParser::method(char c) {
	if (utils::is_ualpha(c))
		_method.push_back(c);
	else if (c == SP) {
		if (_method.size() == 0)
			_bad_request("invalid method: method token begins with space");
		_step = URI;
		Logger::debug("method", _method);
	}
	else if (c == HTAB)
		_bad_request("invalid method/uri separator: horizontal tab");
	else if (c == CR || c == LF)
		_bad_request("missing data on request first line");
	else if (utils::is_lalpha(c))
		_bad_request("method characters must be upper case");
	else
		_bad_request("invalid character at request first line");
}

// URI enconding (tb serve pra POST) - não entendi se posso receber https
// URI normalization:
// normalizing the scheme and host to lowercase
// normalizing the port to remove any leading zeros.
// If port is elided from the URI, the default port for that scheme is used
// o host pode ser um IP, que pode estar em ipv4 ou ipv6...
// It is RECOMMENDED that all senders and recipients support, at a minimum, URIs with lengths of 8000 octets in protocol elements.
void RequestParser::uri(char c) {
	static bool init_uri = true;

	if (c == SP && init_uri)
		return ;
	else if (c == CR || c == LF)
		_bad_request("missing data on request first line");
	else if (init_uri && c == HTAB)
		_bad_request("invalid method/uri separator: horizontal tab");
	if (http::is_uri_char(c)) {
		if (init_uri)
			init_uri = false;
		_uri.push_back(c);
	}
	else if (c == SP) {
		// Logger::debug("uri", _uri);
		Logger::warning("uri", _uri);
		_step = PROTOCOL;
		init_uri = true;
	}
	else
		_invalid_request("invalid uri character", c, http::BAD_REQUEST);
}

// The HTTP version always takes the form "HTTP/x.x", uppercase
void RequestParser::protocol(char c) {
	static bool init_protocol = true;
	// static std::string protocol; // testar se dá pra usar

	if (c == SP && init_protocol)
		return ;
	if (c != SLASH && c != CR && c != LF && c != SP) {
		if (init_protocol)
			init_protocol = false;
		_protocol.push_back(c);
	}
	else if (c == SLASH) {
		if (_protocol.compare(_right_protocol) != 0)
			_invalid_request("invalid protocol", _protocol, http::BAD_REQUEST);
			// throw http::InvalidRequest(http::NOT_FOUND); // nginx
		_step = VERSION;
		Logger::debug("protocol", _protocol);
		init_protocol = true;
	}
	else if (c == SP)
		_bad_request("invalid space in HTTP protocol");
	else if (_protocol.compare(_right_protocol) == 0)
		_bad_request("missing HTTP version");
	else
		_invalid_request("invalid protocol", _protocol, http::BAD_REQUEST);
}

// RFC 9110 - item 2.5. Protocol Version:
// HTTP's version number consists of two decimal digits separated by a "."
// (period or decimal point)
// version format: <major-version>.<minor-version>

// RFC 9110 - item 6.2. Control Data:
// A server can send a 505 (HTTP Version Not Supported) response if it wishes,
// for any reason, to refuse service of the client's major protocol version.
// if major-version != 1: refuse -> 505

// RFC 9110 - item 6.2. Control Data:
// A recipient that receives a message with a major version number that it
// implements and a minor version number higher than what it implements SHOULD
// process the message as if it were in the highest minor version within that
// major version to which the recipient is conformant
// temos que aceitar 1.2, 1.3, 1.11
// não precisamos aceitar 1.0?

void RequestParser::version(char c) {
	int size;

	if (c == SP && !_version.empty())
		return ;
	if (std::isdigit(c) || (c == POINT && !_version.empty()))
		_version.push_back(c);
	else if (c == CR || c == LF) {
		size = _version.size();
		if (size == 3) {
			if (_version[0] == '1' && _version[1] == POINT && _version[2] == '1') {
				Logger::debug("version", _version);
				if (c == CR)
					_step = CR_FIRST_LINE;
				if (c == LF) {
					Logger::debug("LF first line");
					_step = HEADER;
				}
			}
			else
				_invalid_request(
					HTTP_VERSION, _version, http::HTTP_VERSION_NOT_SUPPORTED
				);
		}
		else if (
			size == 0
			|| (size == 1 && _version[0] == '1')
			|| (size == 2 && _version[0] == '1' && _version[1] == POINT)
		)
			_invalid_request(HTTP_VERSION, _version, http::BAD_REQUEST);
		else
			_invalid_request(HTTP_VERSION, _version, http::HTTP_VERSION_NOT_SUPPORTED);
	}
	else
		_invalid_request(HTTP_VERSION, http::BAD_REQUEST);
}

void RequestParser::_check_uri(void) {
	// check if the path exists and set it; get location
	// uri
	// not found here
	// partes da uri:
	// scheme: não vem
	// authority: não vem

// 	The generic syntax uses the slash ("/"), question mark ("?"), and
//    number sign ("#") characters to delimit components that are
//    significant to the generic parser's hierarchical interpretation of an
//    identifier

// se % -> caracter especial com valor em hexadecimal % + 2 caracteres hexadecimais
// For example, "%20" = SP
// "%" HEXDIG HEXDIG
// uppercase e lowercase HEXDIG são iguais
// For consistency, URI producers and
//    normalizers should use uppercase hexadecimal digits for all percent-
//    encodings.

// Reserved Characters:
// reserved    = gen-delims / sub-delims
// gen-delims  = ":" / "/" / "?" / "#" / "[" / "]" / "@" -> delimiting
//    characters that are distinguishable from other data within a URI
// sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"
//                   / "*" / "+" / "," / ";" / "="

//  Unreserved Characters: Characters that are allowed in a URI but do not have a reserved purpose:
// ALPHA / DIGIT / "-" / "." / "_" / "~"

	// se não começa com / mandar um erro -> por hora não vou verificar scheme e authority
	if (_uri[0] != SLASH) // localhost:8080/data
		_invalid_request(
			"URI different from expected. Check it",
			_uri,
			http::INTERNAL_SERVER_ERROR
		); // ver se pode vir sem / no início

	std::vector<t_location> locations = _request->server()->getLocations();
	std::string::iterator it = _uri.begin(), end = _uri.end();
	int i, location_pos = 0, locations_size = locations.size();
	std::string path;

	// get first location:
	std::cout << BLUE << "Checking URI:\n" << GREY;
	path.clear();
	path.push_back(_uri[0]);
	location_pos = 0;
	Logger::debug("First location", locations[0].location);

	// /data -> content/html/index.html -> passar pelos locations do server, compara com o location
	// /data/index2.html -> content/html/data/index2.html
	bool increment = true;
	while (++it != end && !http::uri_path_end(*it)) {
		std::cout << "char path: " << *it << std::endl;
		if (*it == SLASH || (it + 1) == end) {
			std::cout << "Entrou pra procurar um novo location" << std::endl;
			if ((it + 1) == end && *it != SLASH) {
				increment = false;
				path.push_back(*it);
			}
			for (i = 0; i < locations_size; ++i) {
				if (path.compare(locations[i].location) == 0) {
					Logger::debug("location found", locations[i].location);
					location_pos = i;
					break;
				}
			}
		}
		if (increment)
			path.push_back(*it);
	}

	// substitui o location na path pelo root do location
	std::string location = locations[location_pos].location;
	std::string root = locations[location_pos].root;
	Logger::debug("Final path", path);
	Logger::debug("Final location", location);
	if (location.size() > 1)
		path.erase(0, location.size());
	Logger::debug("Final path", path);
	path.insert(path.begin(), root.begin(), root.end());
	Logger::debug("Final path", path);

	// se não tiver ponto => é um diretório => verificar se existe; => not found
	//                                 => se autoindex -> cgi?
	//                                    else index?
	if (!http::is_path_to_file(path)) { // não tem ponto na path?
		DIR *dr;

		dr = opendir(path.c_str());
		if (dr == NULL) // talvez verificar se é um arquivo tb...
			_invalid_request("Directory not found", path, http::NOT_FOUND);
		closedir(dr);
		Logger::debug("Found directory", path);
		if (!locations[location_pos].permit.autoindex) {
			Logger::warning("Index file", *(locations[location_pos].index.begin()));
			path = *(locations[location_pos].index.begin());
		}
		else
			path.push_back(SLASH);
	}
	else { // => é um arquivo => verificar se arquivo existe => not found
		std::ifstream file;

		file.open(path.c_str());
		if (file.fail())
			_invalid_request("Directory not found", path, http::NOT_FOUND);
		file.close();
	}

	_request->setPath(path);
	_request->setUri(_uri); // precisa tirar a parte final se receber parâmetros?
}

void RequestParser::_check_method(void) {
	// verify if method is allowed in specified location:
	try {
		// esse check vai ter que levar em conta o location
		_request->setMethod(http::str_to_enum_method(_method));
	} catch(const utils::GeneralException& e) {
		throw http::InvalidRequest(http::METHOD_NOT_ALLOWED);
	} catch(const std::exception& e) {
		throw e;
	}
}
