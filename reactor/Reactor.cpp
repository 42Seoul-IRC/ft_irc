#include "Reactor.hpp"

void Reactor::init(Server server, EventHandler success_handler, EventHandler error_handler)
{
	if ((kqueue_ = kqueue()) == -1)
		std::runtime_error("Kqueue error");
	
	server_ = server;
	success_handler_ = success_handler;
	error_handler_ = error_handler;
}

void Reactor::addSocket(int socket)
{
	struct kevent event;
	EV_SET(&event, socket, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, 0);
	kevent(kqueue_, &event, 1, 0, 0, 0);
}

void Reactor::run(void)
{
	while(true)
	{
		const int MAX_EVENT = 100;

		struct kevent events[MAX_EVENT];
		int num = kevent(kqueue_, 0, 0, events, MAX_EVENT, 0);

		for (int i = 0; i < num; i++)
		{
			struct kevent* cur_event = &events[i];

			if (cur_event->flags & EV_ERROR)
				error_handler_(cur_event->ident);
			if (cur_event->flags & EV_ERROR)
				error_handler_(cur_event->ident);
			if (cur_event->filter == EVFILT_WRITE)
				error_handler_(cur_event->ident);
			if (cur_event->filter == EVFILT_READ)
				success_handler_(cur_event->ident);
		}
	}
}