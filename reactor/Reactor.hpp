#ifndef REACTOR_HPP
# define REACTOR_HPP

#include <sys/event.h>

#include "../server/Server.hpp"

class Reactor {
	typedef void (Server::*EventHandler)(int);

	private:
		int kqueue_;
		struct kevent* event_;

		Server server_;
		EventHandler success_handler_;
		EventHandler error_handler_;

	public:
		void init(Server server, EventHandler success_handler, EventHandler error_handler);
		void addSocket(int socket);

		void run(void);
};

#endif