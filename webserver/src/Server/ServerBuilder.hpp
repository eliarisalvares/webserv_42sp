#ifndef SERVERBUILDER_HPP
# define SERVERBUILDER_HPP

# include <iostream>
# include <fstream> // using files
# include <vector>
# include <map>

typedef std::map<std::string, std::string>	t_infoServer;

void	getServerInfo(std::vector<std::string> input, int index);

#endif