#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <unistd.h>
#include <iostream> //runtime_error 때문에 넣었는데 다른 대체제
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class Socket {
	private:
		int socket_;

		Socket(void);

	public:
		void bind(char* port);
		void listen(void);

		int getSocket(void);

		int accept(void);
};

#endif