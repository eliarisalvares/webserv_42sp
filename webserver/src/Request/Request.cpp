/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 19:30:54 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/06 23:23:45 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void): _method(GET) { }

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

requestMethod	Request::getMethod(void) const {
	return this->_method;
}
