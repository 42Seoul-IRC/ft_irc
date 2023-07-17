#include "Server.hpp"

void Server::init(char* port, char* password)
{
	try
	{
		server_socket_.bind(port);
		server_socket_.listen();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	socket_reactor_.init(&Server::successHandler, &Server::errorHandler);
	socket_reactor_.addSocket(server_socket_.getSocket());

	packet_manager_.init(password);
}

void Server::successHandler(int socket)
{
	if (server_socket_.getSocket() == socket)
	{
		int client_socket = server_socket_.accept();
		if (client_socket == -1)
			std::runtime_error("Socket accept error");
		
		if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
			std::runtime_error("Client fcntl error");

		if (session_manager_.registerSession(client_socket) == -1)
		{
			close(client_socket);
			return ;
		}
		socket_reactor_.addSocket(client_socket);
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

void Server::errorHandler(int socket)
{
	if (server_socket_.getSocket() == socket)
	{
		std::runtime_error("Server socket event error");
	}
	else
	{
		Session* session = session_manager_.getSessionBySocket(socket);
		if (session)
		{
			Message message;
			message.command_ = "QUIT";

			packet_manager_.process(session.getSessionIndex(), message);
			session_manager_.removeSessionBySocket(socket);
		}
		else
		{
			close(socket);
		}
	}
}

void Server::start(void)
{
	while(true)
	{
		socket_reactor_.run();
		process();
	}
}

void Server::process(void)
{
	for (std::deque<Packet>::iterator it = packet_queue.begin(); it != packet_queue.end(); it++)
	{
		packet_manager_.process(*it);
	}
	packet_queue.clear();
}