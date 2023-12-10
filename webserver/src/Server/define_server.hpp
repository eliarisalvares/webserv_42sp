/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define_server.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/10 11:18:04 by feralves          #+#    #+#             */
/*   Updated: 2023/12/10 11:18:05 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_SERVER_HPP
# define DEFINE_SERVER_HPP

# define BUFFSIZE 256
# define CLIENT_MAX_BODY_SIZE 1048576
# define CGI_LOCATION "/cgi"
# define CGI_EXECUTOR "python3"
# define ERROR_PAGES "404 404.html"
# define ERROR_LOCATION "content/error_pages/"
# define LOCATION "/"
# define METHODS "GET, POST, DELETE"
# define ROOT "content"
# define SERVER_NAME "WebWizards"
# define SERVER_PORT "8080"
# define TIMEOUT 5000
# define DEFAULT_UPLOAD "content/upload"

#endif
