/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:44:59 by feralves          #+#    #+#             */
/*   Updated: 2023/11/07 16:57:11 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParserConfig.hpp"

ParserConfig::ParserConfig(void) { }

ParserConfig::~ParserConfig(void) { }

ParserConfig::ParserConfig(ParserConfig const& copy) { (void)copy; }

ParserConfig const& ParserConfig::operator=(ParserConfig const & copy) {
	(void)copy;
	return *this;
}

void	ParserConfig::readFile(std::string fileName) {
	//get infos from file
}
