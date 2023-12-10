/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_exceptions.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/10 11:17:44 by feralves          #+#    #+#             */
/*   Updated: 2023/12/10 11:17:45 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_builder.hpp"

const char* PortNotFoundErrorExeption::what() const throw() {
	return ("Invalid Port found.");
}

const char* PortNeedsSudoExeption::what() const throw() {
	return ("Sudo required to run.");
}

const char* CGIWrongArgumentException::what() const throw() {
	return ("Wrong number of arguments for CGI.");
}

const char* CGIInvalidException::what() const throw() {
	return ("CGI mode not supported by webserv.");
}

const char* MethodsWrongArgumentException::what() const throw() {
	return ("Wrong number of arguments for Allowed Methods.");
}

const char* TooLargeException::what() const throw() {
	return ("Size of client body too large.");
}

const char* BufferSizeInvalidException::what() const throw() {
	return ("Not a valid argument for Buffer size.");
}

const char* LocationNotOpenedException::what() const throw() {
	return ("Location open bracket '{' not found.");
}

const char* ErrPagesWrongArgumentException::what() const throw() {
	return ("Wrong number of arguments to set Error Pages.");
}

const char* ErrPagesInvalidException::what() const throw() {
	return ("Not a valid argument for Error Pages.");
}

const char* InvalidFileException::what() const throw() {
	return ("Invalid File");
}

const char* AutoIndexWrongArgumentException::what() const throw() {
	return ("Wrong number of arguments for autoindex.");
}

const char* AutoIndexInvalidException::what() const throw() {
	return ("Not a valid argument for autoindex.");
}

const char* DirListWrongArgumentException::what() const throw() {
	return ("Wrong number of arguments for directory_listing.");
}

const char* DirListInvalidException::what() const throw() {
	return ("Not a valid argument for directory_listing.");
}

const char* PortAlreadyInUseException::what() const throw() {
	return ("Port is already in use, check if there is another server working.");
}

const char* RedirWrongArgumentException::what() const throw() {
	return ("Wrong number of arguments for redirect.");
}