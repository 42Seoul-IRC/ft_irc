#include "Server.hpp"

Server::Server()
{
}

void Server::init(char* port, char* password)
{
	server_socket_.bind(port);
	server_socket_.listen();
	if ((kqueue_ = kqueue()) == -1)
		throw std::runtime_error("Kqueue error");

	addSocket(server_socket_.getSocket());

	packet_manager_.init(password);
}

void Server::addSocket(int socket)
{
	struct kevent event;
	EV_SET(&event, socket, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, 0);
	kevent(kqueue_, &event, 1, 0, 0, 0);
}

void Server::run(void)
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
			if (cur_event->flags & EV_EOF)
				errorHandler(cur_event->ident);
			if (cur_event->filter == EVFILT_WRITE)
				errorHandler(cur_event->ident);
			if (cur_event->filter == EVFILT_READ)
				successHandler(cur_event->ident);
		}
		process();
	}
}

void Server::process(void)
{
	for (std::deque<Packet>::iterator it = packet_queue.begin(); it != packet_queue.end(); it++)
	{
		packet_manager_.execute(*it);
	}
	packet_queue.clear();
}

void Server::successHandler(int socket)
{
	if (server_socket_.getSocket() == socket)
	{
		int client_socket = server_socket_.accept();
		if (client_socket == -1)
			throw std::runtime_error("Socket accept error");

		if (::fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
			throw std::runtime_error("Client fcntl error");

		try
		{
			packet_manager_.client_manager_.addClientBySocket(client_socket);
			addSocket(client_socket);
			std::cout << "[INFO] Client connected : " << client_socket << ", " << packet_manager_.client_manager_.getClientBySocket(client_socket) << std::endl;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			close(client_socket);
		}
	}
	else
	{
		Client * client = packet_manager_.client_manager_.getClientBySocket(socket);
		if (client)
		{
			char tmp[512];

			memset(tmp, 0, sizeof(tmp));
			int size = recv(socket, tmp, sizeof(tmp) - 1, 0);
			if (size < 1)
			{
				packet_manager_.removeClientBySocket(socket);
				close(socket);
				return ;
			}
			tmp[size] = '\0';

			std::string str(tmp);
			std::string::size_type idx = str.rfind("\r\n");
			if (idx == std::string::npos)
			{
				client->buffer_ << tmp;
				return ;
			}

			std::vector<Message> messages = Message::parse(client->buffer_.str() + str.substr(0, idx + 2));
			client->buffer_.str(str.substr(idx + 2));

			for (std::vector<Message>::iterator it = messages.begin(); it != messages.end(); it++)
			{
				std::cout << "[LOG] {" << socket << " -> irc.webserv} - " << (*it).toString();
			 	struct Packet packet = {socket, *it};
				packet_queue.push_back(packet);
			}
		}
		else
		{
			close(socket);
			std::cout << "[INFO] Client disconnected : " << socket << std::endl;
		}
	}
}

void Server::errorHandler(int socket)
{
	if (server_socket_.getSocket() == socket)
	{
		throw std::runtime_error("Server socket event error");
	}
	else
	{
		Client * client = packet_manager_.client_manager_.getClientBySocket(socket);
		if (client)
		{
			Message message;
			message.setCommand("QUIT");
			
			struct Packet quit = {socket, message};

			packet_manager_.execute(quit);
			std::cout << "[INFO] Client disconnected : " << socket << std::endl;
		}
		else
		{
			close(socket);
			std::cout << "[INFO] Client disconnected : " << socket << std::endl;
		}
	}
}
