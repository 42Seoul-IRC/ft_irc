#include "Socket.hpp"

Socket::Socket(void)
{
	if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		std::runtime_error("Socket error");

	int option = 1;
	if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
		std::runtime_error("Setsocket error");
	
	if (fcntl(socket_, F_SETFL, O_NONBLOCK) == -1)
		std::runtime_error("Fcntl error");
}

void Socket::bind(char *argv)
{
	char *end;
	long port = strtol(argv, &end, 10);

	if (*end != '\0')
		std::runtime_error("Port error");
	if (port < 0 || port > 65535)
		std::runtime_error("Port error");

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if (::bind(socket_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
		std::runtime_error("Bind error");
}

void Socket::listen(void)
{
	if (::listen(socket_, 5) == -1)
		std::runtime_error("Listen error");
}

int Socket::getSocket(void)
{
	return socket_;
}

int Socket::accept(void)
{
	return ::accept(socket_, 0, 0);
}