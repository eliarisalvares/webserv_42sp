/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_builder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elraira- <elraira-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/10 11:17:54 by feralves          #+#    #+#             */
/*   Updated: 2023/12/14 00:05:05 by elraira-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_builder.hpp"

int	obtainPort(std::vector<std::string> input, int index) {
	int		port = -1;

	if (input[index].substr(0, 7) == "listen ") {
		port = ftstring::strtoi(input[index].substr(7));
		if (port < 1 || port > 65535)
			throw PortNotFoundErrorExeption();
		if (port < 1024)
			throw PortNeedsSudoExeption();
		Logger::debug("Port setted", port);
	}
	return port;
}

int	obtainBodySize(std::vector<std::string> input, int index) {
	int							bodySize = CLIENT_MAX_BODY_SIZE;
	std::string					bodySizeString;
	std::vector<std::string>	words;

	if (input[index].substr(0, 21) == "client_max_body_size ") {
		words = ftstring::split(input[index].substr(21), ' ');
		bodySizeString = words[0];
		bodySize = ftstring::strtoi(bodySizeString);
		switch (bodySizeString.at(bodySizeString.length() - 1))
		{
		case 'M':
			bodySize *= 1048576;
			break;
		case 'K':
			bodySize *= 1024;
			break;
		default:
			break;
		}
		if (bodySize > 1048576)
			throw TooLargeException();
		Logger::debug("client_max_body_size setted", bodySize);
	}
	return bodySize;
}

int	obtainBufferSize(std::vector<std::string> input, int index) {
	int							bufferSize = BUFFSIZE;

	if (input[index].substr(0, 12) == "buffer_size ") {
		bufferSize = ftstring::strtoi(input[index].substr(12));
		if (bufferSize < 1 || bufferSize > 1024)
			throw BufferSizeInvalidException();
		Logger::debug("Buffer Size setted", bufferSize);
	}
	return bufferSize;
}

std::string	obtainRoot(std::vector<std::string> input, int index) {
	std::string					root;
	std::vector<std::string>	words;
	DIR *dr;

	if (input[index].substr(0, 5) == "root ") {
		words = ftstring::split(input[index].substr(5), ' ');
		if (input[index].substr(5, 2) == "/")
			root = ROOT;
		else
			root = ROOT + words[0];
		dr = opendir(root.data());
		if (dr == NULL) {
			throw utils::GeneralException(utils::INVALID_DIRECTORY);
		}
		closedir(dr);
		Logger::debug("Root setted", root);
	}
	return root;
}

std::string	obtainRedirect(std::vector<std::string> input, int index) {
	std::string					redir;
	std::vector<std::string>	words;

	if (input[index].substr(0, 9) == "redirect ") {
		words = ftstring::split(input[index].substr(9), ' ');
		if (words.size() != 1)
			throw RedirWrongArgumentException();
		redir = words[0];
		Logger::debug("Redirect setted", redir);
	}
	return redir;
}

bool	obtainCGI(std::vector<std::string> input, int index) {
	std::string					name;
	std::vector<std::string>	cgiName;
	bool						valid = true;

	if (input[index].substr(0, 4) == "cgi ") {
		name = input[index].substr(4);
		cgiName = ftstring::split(name, ' ');
		if (cgiName.size() != 2) {
			valid = false;
			throw CGIWrongArgumentException();
		}
		else if (cgiName[0] != ".py" || cgiName[1] != "python3") {
			valid = false;
			throw CGIInvalidException();
		}
		Logger::debug("CGI setted from .conf file");
	}
	return valid;
}

bool	obtainAutoIndex(std::vector<std::string> input, int index) {
	std::vector<std::string>	autoindex;
	bool						valid = false;

	if (input[index].substr(0, 10) == "autoindex ") {
		autoindex = ftstring::split(input[index].substr(10), ' ');
		if (autoindex.size() != 1)
			throw AutoIndexWrongArgumentException();
		if (autoindex[0] == "true")
			valid = true;
		else if (autoindex[0] == "false")
			valid = false;
		else
			throw AutoIndexInvalidException();
		Logger::debug("Autoindex setted as ", autoindex[0]);
	}
	return valid;
}

std::vector<std::string>	obtainName(std::vector<std::string> input, int index) {
	std::string					name;
	std::vector<std::string>	serverName;

	if (input[index].substr(0, 7) == "server_name ") {
		name = input[index].substr(7);
		serverName = ftstring::split(name, ' ');
		Logger::debug("Server name setted", name);
	}
	return serverName;
}

std::set<std::string>	obtainMethod(std::vector<std::string> input, int index) {
	std::set<std::string>		methods;
	std::vector<std::string>	words;

	if (input[index].substr(0, 16) == "allowed_methods ") {
		words = ftstring::split(input[index].substr(16), ' ');
		if (words.size() <= 0 || words.size() >= 4)
			throw MethodsWrongArgumentException();
		for (size_t j = 0; j < words.size(); j++) {
			if (words[j] == "GET" || words[j] == "POST" || words[j] == "DELETE")
				methods.insert(words[j]);
			else
				throw utils::GeneralException(utils::INVALID_METHOD);
		}
		Logger::debug("Allowed Methods setted from .conf file");
	}
	return methods;
}

std::pair<int, std::string>	obtainErrorPages(std::vector<std::string> input, int index) {
	std::pair<int, std::string>	paired;
	std::vector<std::string>	words;
	std::string					page;
	int							nbr;
	char const		*values[] = {"400", "401", "402", "403", "404", "405", "406", "407", "408", "409", "410",
								"411", "412", "413", "414", "415", "416", "417", "418", "421", "422", "423",
								"424", "425", "426", "428", "429", "431", "451", "500", "501", "502", "503",
								"504", "505", "506", "507", "508", "510", "511"};
	std::vector<std::string> const codes(values, (values + sizeof(values) / sizeof(values[0])));
	std::vector<std::string>::const_iterator it;

	if (input[index].substr(0, 11) == "error_page ") {
		words = ftstring::split(input[index].substr(11), ' ');
		if (words.size() != 2)
			throw ErrPagesWrongArgumentException();
		it = words.begin();
		if (!(std::find(codes.begin(), codes.end(), *it) != codes.end()))
			throw ErrPagesInvalidException();
		nbr = ftstring::strtoi(words[0]);
		page = ERROR_LOCATION + words[1];
		if (!checkFileWorks(page))
			throw InvalidFileException();
		paired = std::make_pair(nbr, page);
		Logger::debug("Error page setted, value", nbr);
	}
	return paired;
}

std::set<std::string>	obtainIndex(std::vector<std::string> input, int index) {
	std::set<std::string>		value;
	std::vector<std::string>	words;
	std::string					page;

	if (input[index].substr(0, 6) == "index ") {
		words = ftstring::split(input[index].substr(6), ' ');
		for (size_t j = 0; j < words.size(); j++) {
			page = ROOT "/" + words[j];
			if (!checkFileWorks(page))
				throw InvalidFileException();
			value.insert(page);
		}
		Logger::debug("Index setted", input[index].substr(6));
	}
	return value;
}

std::set<std::string>	obtainIndex(std::vector<std::string> input, int index, std::string root) {
	std::set<std::string>		value;
	std::vector<std::string>	words;
	std::string					page;

	if (input[index].substr(0, 6) == "index ") {
		words = ftstring::split(input[index].substr(6), ' ');
		for (size_t j = 0; j < words.size(); j++) {
			page = root + "/" + words[j];
			if (!checkFileWorks(page))
				throw InvalidFileException();
			value.insert(page);
		}
		Logger::debug("Index setted", page);
	}
	return value;
}

std::set<std::string>	getRootIndex(std::string root) {
	std::set<std::string>		value;
	std::string					page;

	page = root + "/index.html";
	if (!checkFileWorks(page))
		page = "content/index.html";
	value.insert(page);
	return value;
}
