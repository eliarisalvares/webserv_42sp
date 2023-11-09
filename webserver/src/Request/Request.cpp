/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 19:30:54 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/09 10:19:41 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void): _fd(0), _method(GET) { }
Request::Request(int fd): _fd(fd), _method(GET) { }

Request::~Request(void) { }

Request::Request(Request const& copy) {
	*this = copy;
}

Request const& Request::operator=(Request const & copy) {
	if (this != &copy) {
		this->_method = copy.getMethod();
		// this->server = copy.getServer();
	}
	return *this;
}

requestMethod Request::getMethod(void) const {
	return this->_method;
}

int Request::fd(void) const {
	return this->_fd;
}
