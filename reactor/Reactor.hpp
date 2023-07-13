#ifndef REACTOR_HPP
# define REACTOR_HPP

#include <sys/event.h>

#include "../server/Server.hpp"

class Reactor {
	typedef void (Server::*EventHandler)(int);

	private:
		int kqueue_;
		struct kevent* event_;

		Server target_;
		EventHandler event_success_;
		EventHandler event_error_;
};

#endif