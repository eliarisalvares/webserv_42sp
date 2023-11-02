/*
Server Class
*/

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <cstdlib> // exit

class Server {
public:
	Server(void);
	~Server(void);
	Server(Server const& copy);
	Server const& operator=(Server const& copy);

private:

};

#endif