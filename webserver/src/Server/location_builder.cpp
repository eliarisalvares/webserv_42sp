/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location_builder.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/10 11:18:00 by feralves          #+#    #+#             */
/*   Updated: 2023/12/11 18:47:11 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_builder.hpp"

t_location	initLocation(void) {
	t_location		location;

	location.allowed_methods = http::methods;
	location.location = LOCATION;
	location.root = ROOT;
	location.index.insert("content/index.html");
	location.permit.autoindex = false;
	location.permit.has_redir = false;
	location.permit.redirExternal = false;
	return (location);
}



t_location	obtainLoc(std::vector<std::string> input, int index) {
	t_location					location;
	std::vector<std::string>	locName;
	bool						gotRoot = false;
	bool						gotIndex = false;

	location = initLocation();
	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr(0, 9) == "location ") {
			locName = ftstring::split(input[i].substr(9), ' ');
			Logger::debug("Init location parsing", location.location);
			location.location = locName[0];
			if (locName[1] != "{")
				throw LocationNotOpenedException();
		}
		if (!gotRoot) {
			size_t	j = i;
			while (input[j] != "}") {
				if (input[j].substr(0, 5) == "root "){
					location.root = obtainRoot(input, j);
					gotRoot = true;
				}
				if (input[j] == "}")
					break ;
				j++;
			}
		}
		if (input[i].substr(0, 16) == "allowed_methods ")
			location.allowed_methods = obtainMethod(input, i);
		if (input[i].substr(0, 6) == "index ") {
			gotIndex = true;
			location.index = obtainIndex(input, i, location.root);
		}
		if (input[i].substr(0, 10) == "autoindex ")
			location.permit.autoindex = obtainAutoIndex(input, i);
		if (input[i].substr(0, 4) == "cgi ")
			location.cgi = obtainCGI(input, i);
		if (input[i].substr(0, 11) == "error_page ") {
			std::pair<int, std::string> paired = obtainErrorPages(input, i);
			if (location.error_pages.find(paired.first) != location.error_pages.end())
				location.error_pages[paired.first] = paired.second;
			else
				location.error_pages.insert(paired);
		}
		if (input[i].substr(0, 9) == "redirect ") {
			location.permit.has_redir = true;
			location.redirection = obtainRedirect(input, i);
			if (location.redirection.substr(0, 1) != "/")
				location.permit.redirExternal = true;
		}
		if (input[i] == "}")
			break ;
	}
	if (!gotIndex)
		location.index = getRootIndex(location.root);
	Logger::debug("Location saved", location.location);
	return (location);
}