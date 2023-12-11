/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_control_data.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 00:28:10 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/11 18:18:18 by sguilher         ###   ########.fr       */
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
		Logger::debug("uri", _uri);
		_step = PROTOCOL;
		init_uri = true;
	}
	else
		_invalid_request("invalid uri character", c, http::BAD_REQUEST);
}

// The HTTP version always takes the form "HTTP/x.x", uppercase
void RequestParser::protocol(char c) {
	static bool init_protocol = true;

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
void RequestParser::_check_uri(void) {
	Logger::debug("******************* Checking URI *******************");
	if (_uri[0] != SLASH)
		_invalid_request(
			"URI different from expected. Check it",
			_uri,
			http::INTERNAL_SERVER_ERROR
		);

	Server* server = _request->server();
	std::vector<t_location>* locations = server->getLocations();
	std::string::iterator it = _uri.begin(), end = _uri.end();
	int i, location_pos = -1, locations_size = locations->size();
	bool use_server_config = true;
	std::string path, location_str;
	t_location *location;
	location_str.clear();
	path.clear();
	path.push_back(*it);

	if (locations_size)
		use_server_config = false;

	bool increment = true, is_first_iteration = true;
	while (it != end && !http::uri_path_end(*it)) { // arrumar o path
		if (*it == SLASH || (it + 1) == end) {
			if ((it + 1) == end && *it != SLASH) {
				increment = false;
				path.push_back(*it);
			}
			if (!use_server_config) {
				for (i = 0; i < locations_size; ++i) {
					if (path.compare((*locations)[i].location) == 0) {
						Logger::debug("location found", (*locations)[i].location);
						location_pos = i;
						location = &(*locations)[i];
						_request->setLocation(location);
						break;
					}
				}
			}
		}
		if (increment && !is_first_iteration)
			path.push_back(*it);
		if (is_first_iteration)
			is_first_iteration = false;
		++it;
	}
	Logger::debug("Path from URI", path);

	std::string root;
	bool is_redirect = false, is_external_redirection = false;
	if (location_pos == -1) {
		use_server_config = true;
		is_redirect = server->getPermit().has_redir;
		is_external_redirection = server->getPermit().redirExternal;
		if (is_redirect)
			root = server->getRedirect();
		else
			root = server->getRoot();
	}
	if (location_pos > -1) {
		location_str = location->location;
		is_redirect = location->permit.has_redir;
		is_external_redirection = location->permit.redirExternal;
		if (is_redirect)
			root = location->redirection;
		else
			root = location->root;
		if (is_external_redirection)
			path = location->redirection;
	}
	if (is_redirect)
		Logger::debug("redirect", root);
	else
		Logger::debug("root", root);
	if (is_external_redirection)
		path = root;
	else {
		if (location && location_str.size() > 0)
			path.erase(0, location_str.size());
		path.insert(path.begin(), root.begin(), root.end());
	}
	Logger::debug("Path with changing root", path);

	if (is_redirect) {
		// se external, só salva o externo
		Logger::debug("Redirection", path);
		_request->setStatusCode(http::MOVED_PERMANENTLY);
		_request->setPath(path);
		_request->setUri(_uri);
		return ;
	}

	if (!http::is_path_to_file(path)) {
		DIR *dr = opendir(path.c_str());
		if (dr == NULL)
			_invalid_request("Directory not found", path, http::NOT_FOUND);
		closedir(dr);
		Logger::debug("Found directory", path);
		if (use_server_config) {
			if (!server->getAutoindex()) {
				path = *(server->getIndex().begin());
				Logger::debug("Index file", path);
			}
			else {
				if (path[path.size() - 1] != SLASH)
					path.push_back(SLASH);
			}
		}
		else {
			if (!location->permit.autoindex) {
				path = *(location->index.begin());
				Logger::debug("Index file", path);
			}
			else {
				if (path[path.size() - 1] != SLASH)
					path.push_back(SLASH);
			}
		}
	}
	else {
		std::ifstream file;
		file.open(path.c_str());
		if (file.fail())
			_invalid_request("File not found", path, http::NOT_FOUND);
		file.close();
	}

	_request->setPath(path);
	_request->setUri(_uri);
}

// std::string RequestParser::_get_root_redirect(std::string const& redirect) {
// 	Logger::warning("Enter redirection", redirect);
// 	std::vector<t_location>* locations = _request->server()->getLocations();
// 	std::string::const_iterator it = redirect.begin(), end = redirect.end();
// 	int i, locations_size = locations->size();
// 	bool use_server_config = locations_size ? false : true;
// 	std::string path, location_str;
// 	t_location *location;
// 	location_str.clear();
// 	path.clear();

// 	bool increment = true;
// 	while (it != end) {
// 		if (*it == SLASH || (it + 1) == end) {
// 			if ((it + 1) == end && *it != SLASH) {
// 				increment = false;
// 				path.push_back(*it);
// 			}
// 			if (!use_server_config) {
// 				for (i = 0; i < locations_size; ++i) {
// 					if (path.compare((*locations)[i].location) == 0) {
// 						Logger::debug("location found", (*locations)[i].location);
// 						location = &(*locations)[i];
// 						_request->setLocation(location);
// 						break;
// 					}
// 				}
// 			}
// 		}
// 		if (increment)
// 			path.push_back(*it);
// 		++it;
// 	}
// 	Logger::debug("Path from redirect", path);
// 	return path;
// }

void RequestParser::_check_method(void) {
	std::set<std::string> allowed_methods;
	std::set<std::string>::iterator it;

	if (_request->location())
		allowed_methods = _request->location()->allowed_methods;
	else
		allowed_methods = _request->server()->getMethods();
	it = allowed_methods.find(_method);
	if (it == allowed_methods.end())
		throw http::InvalidRequest(http::METHOD_NOT_ALLOWED);
	_request->setMethod(http::str_to_enum_method(_method));
}
