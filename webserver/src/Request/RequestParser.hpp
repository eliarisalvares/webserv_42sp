/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 16:34:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/11 17:43:47 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER
# define REQUEST_PARSER

# include <map>
# include <string>

// ABNF Rules

// Special chars
# define CR '\r'
# define LF '\n'


class RequestParser {
public:
	RequestParser(void);
	RequestParser(RequestParser const& copy);
	RequestParser& operator=(RequestParser const& copy);
	~RequestParser(void);

	typedef enum e_steps {
		INIT,
		FIRST_LINE,  // CRLF?
		METHOD,
		URI,
		PROTOCOL,
		VERSION,
		CRLF,
		HEADER,
		END_HEADER,
		BODY,
		END_BODY,  // necessary?
		END
	}           t_steps;

	e_steps parse();

private:
	t_steps								_parser_step;
	std::map<std::string, std::string>	_parse_result;
};

#endif
