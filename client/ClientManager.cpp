#include "ClientManager.hpp"

void	ClientManager::addClientBySocket(int socket)
{
	Client	*temp;

	temp = new Client(socket);
	socket_clients_.insert(std::make_pair(socket, temp));
}

Client	*ClientManager::getClientBySocket(int socket)
{
	if (socket_clients_.find(socket) == socket_clients_.end())
		return (NULL);
	return (socket_clients_.find(socket)->second);
}
