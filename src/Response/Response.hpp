/*
Response Class
*/

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <cstdlib> // exit

class Response {
public:
	Response(void);
	~Response(void);
	Response(Response const& copy);
	Response const& operator=(Response const& copy);

private:

};

#endif