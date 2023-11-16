#include "ServerBuilder.hpp"

void	getServerInfo(std::vector<std::string> input, int index) {
	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr() == "}")
			break ;
		std::cout << "line " << i << " input : " << input[i] << std::endl;
	}
}