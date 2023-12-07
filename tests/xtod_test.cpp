/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xtod_test.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/06 20:49:48 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/06 22:31:03 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "utils.hpp"
# include "Logger.hpp"

void char_test(std::string values, int expected) {
	std::string::iterator it, end = values.end();
	int result;

	for (it = values.begin(); it != end; ++it) {
		result = utils::xtod(*it);
		std::cout << GREY << *it << " = " << result << ": ";
		if (result == expected)
			std::cout << GREEN << "OK" << RESET << std::endl;
		else
			std::cout << ORANGE << "KO" << RESET << std::endl;
		++expected;
	}
}

void str_test(std::string number, size_t expected) {
	size_t result = 0;

	result = utils::xtod(number);
	std::cout << GREY << number << " = " << result << ": ";
	if (result == expected)
		std::cout << GREEN << "OK" << RESET << std::endl;
	else
		std::cout << ORANGE << "KO" << RESET << std::endl;
}

int main(void) {
	Logger::info("checking char returned value:");
	char_test("0123456789", 0);
	char_test("abcdef", 10);
	char_test("ABCDEF", 10);

	Logger::info("checking str returned value:");
	str_test("B", 11);
	str_test("10", 16);
	str_test("3E8", 1000);
	str_test("100", 256);
	str_test("40D40BA3E3", 278435439587);

	return 0;
}
