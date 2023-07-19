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

void ClientManager::printAllClients()
{
	std::map<int, Client*>::iterator it;

	int i = 0;
	for (it = socket_clients_.begin(); it != socket_clients_.end(); it++)
	{
		std::cout << "--------------------------" << std::endl;
		std::cout << "client " << i << std::endl;

		std::cout << "--------------------------" << std::endl;
		std::cout << "socket: " << it->first << std::endl;
		std::cout << "nickname: " << it->second->getNickName() << std::endl;
		std::cout << "username: " << it->second->getUserName() << std::endl;
		std::cout << "hostname: " << it->second->getHostName() << std::endl;
		std::cout << "channels: " << std::endl;
		std::set<std::string>::iterator it2;
		for (it2 = it->second->getChannels().begin(); it2 != it->second->getChannels().end(); it2++)
			std::cout << *it2 << std::endl;
		std::cout << "--------------------------" << std::endl;
		std::cout << std::endl;
		i++;
	}
}