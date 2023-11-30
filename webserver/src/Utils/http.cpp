/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 21:14:50 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/29 23:46:12 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.hpp"


namespace http {

std::set<std::string> _fill_methods(void) {
	std::string methods_names[] = {
		"GET",
		"POST",
		"PUT",
		"PATCH",
		"DELETE",
		"HEAD",
		"CONNECT",
		"OPTIONS",
		"TRACE",
	};
	std::set<std::string> methods (methods_names, methods_names + 9);
	return methods;
}

// headers that needs a validation - não sei se isso vai ser útil
std::set<std::string> _fill_headers(void) {
	std::string headers_names[] = {
		"Host", // obrigatório, se não tiver retorna 400 - Bad Request
		"Content-Length",  // número
		"Content-Type",  // ?
	};
	std::set<std::string> headers (headers_names, headers_names + 3);
	return headers;
}

}
