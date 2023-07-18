#include "Reactor.hpp"

void Reactor::init(int server_socket)
{
	if ((kqueue_ = kqueue()) == -1)
		throw std::runtime_error("Kqueue error");
	server_socket_ = server_socket;
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
				errorHandler(cur_event->ident);
			if (cur_event->flags & EV_ERROR)
				errorHandler(cur_event->ident);
			if (cur_event->filter == EVFILT_WRITE)
				errorHandler(cur_event->ident);
			if (cur_event->filter == EVFILT_READ)
				successHandler(cur_event->ident);
		}
	}
}

void Reactor::successHandler(int socket)
{
	if (server_socket_ == socket)
	{
		int client_socket = ::accept(server_socket_, 0, 0);
		if (client_socket == -1)
			throw std::runtime_error("Socket accept error");
		
		if (::fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
			throw std::runtime_error("Client fcntl error");

		Client(client_socket);
		// if (session_manager_.registerSession(client_socket) == -1)
		// {
		// 	close(client_socket);
		// 	return ;
		// }
		// socket_reactor_.addSocket(client_socket);
	}
	else
	{
		Session* session = session_manager_.getSessionBySocket(socket);
		if (session)
		{
			session->readString();
		}
	}
}

void Reactor::errorHandler(int socket)
{
	if (server_socket_.getSocket() == socket)
	{
		throw std::runtime_error("Server socket event error");
	}
	else
	{
		Session* session = session_manager_.getSessionBySocket(socket);
		if (session)
		{
			Message message;
			message.command_ = "QUIT";

			packet_manager_.process(session.getSessionIndex(), message);
			// session_manager_.removeSessionBySocket(socket);
		}
		else
		{
			close(socket);
		}
	}
}