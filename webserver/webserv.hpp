/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:08:43 by feralves          #+#    #+#             */
/*   Updated: 2023/11/02 11:09:30 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream> // Para std::cout
# include <sys/socket.h> // Para socket(), bind(), listen(), accept()
# include <netinet/in.h> // Para struct sockaddr_in
# include <unistd.h> // Para read(), close()
# include <sys/epoll.h> // Para epoll_create(), epoll_ctl(), epoll_wait()

# define PORT 8080

#endif