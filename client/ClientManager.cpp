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

<<<<<<< HEAD
Client	*ClientManager::getClientByNick(std::string nick)
{
	if (nick_clients_.find(nick) == nick_clients_.end())
		return (NULL);
	return (nick_clients_.find(nick)->second);
}

bool	ClientManager::clientIsAuth(int socket)
{
	Client	*temp;

	temp = getClientBySocket(socket);
	if (temp == NULL)
		return (false);
	return (temp->getIsAuthenticated());
}

bool	ClientManager::clientIsInServer(std::string nick)
{
	std::map<std::string, Client*>::iterator it;

	for (it = nick_clients_.begin(); it != nick_clients_.end(); it++)
	{
		if (it->first == nick)
			return (true);
	}
	return (false);
}

bool	ClientManager::isValidNickname(std::string nick)
{
	if (nick.length() > 9)
		return (false);
	if (nick.find_first_not_of(FIRST_CHARACTER) != std::string::npos)
		return (false);
	for (std::string::iterator it = nick.begin(); it != nick.end(); it++)
	{
		if (!std::isprint(*it))
			return (false);
	}
	
	return (true);
}

bool	ClientManager::isUsedNickname(std::string nick)
{
	if (nick_clients_.find(nick) == nick_clients_.end())
		return (false);
	return true;
}

void	ClientManager::addNickClient(std::string nick, Client* client)
{
	nick_clients_.insert(std::make_pair(nick, client));
}

void	ClientManager::removeNickClient(std::string nick)
{
	nick_clients_.erase(nick);
}

void	ClientManager::addChannelToClient(std::string nick, std::string channel)
{
	if (nick_clients_.find(nick) == nick_clients_.end())
		return ;
	nick_clients_.find(nick)->second->addChannel(channel);
}

void	ClientManager::removeChannelFromClient(std::string nick, std::string channel)
{
	if (nick_clients_.find(nick) == nick_clients_.end())
		return ;
	nick_clients_.find(nick)->second->deleteChannel(channel);
}
=======
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
>>>>>>> 95007fd22122f80160a7f867ae0e77431d834e97
