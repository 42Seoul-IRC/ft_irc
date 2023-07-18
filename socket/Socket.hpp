#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class Socket {
	private:
		int socket_;


	public:
		Socket(void);
		void bind(char* port);
		void listen(void);

		int getSocket(void);
		int	accept(void);
};

#endif