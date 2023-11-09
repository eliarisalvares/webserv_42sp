/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuilder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 23:00:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/07 20:52:12 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestBuilder.hpp"

RequestBuilder::RequestBuilder(void): _fd(0) {}

RequestBuilder::~RequestBuilder(void) {}

RequestBuilder::RequestBuilder(RequestBuilder const& copy) {
	*this = copy;
}

RequestBuilder const& RequestBuilder::operator=(RequestBuilder const& copy) {
	if (this != &copy) {
		;
	}
	return *this;
}


void RequestBuilder::addRequestData(std::string const newData) {
	_requestData.push_back(newData);
	// quebrar em char
	// análise parcial - status - error, response
}

Request* RequestBuilder::build() {
	Logger log;
	this->parse();
	Request *req = new Request();
	log.debug("Request pointer:");
	std::cout << req << std::endl;
	return req;
}
  // params: Server, fd da conexão


void RequestBuilder::parse() {}

