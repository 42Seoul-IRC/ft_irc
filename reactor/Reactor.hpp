#ifndef REACTOR_HPP
# define REACTOR_HPP

#include <sys/event.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdexcept>

class Reactor {
	private:
		
		int server_socket_;
		// std::map<int, Client> clinets;
		// std::map<int, string> 

	public:
		void init(int server_socket);
		void addSocket(int socket);
		// void successHandler(int socket);
		// void errorHandler(int socket);
		void run(void);
};

#endif