#include "ClientManager.hpp"
#include <iostream>

ClientManager::~ClientManager()
{
	std::map<int, Client*>::iterator it;

	for (it = socket_clients_.begin(); it != socket_clients_.end(); it++)
	{
		it->second->removeClient();
		delete it->second;
	}
	socket_clients_.clear();
	nick_clients_.clear();
}

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

Client	*ClientManager::getClientByNick(const std::string& nick)
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

bool	ClientManager::clientIsInServer(const std::string& nick)
{
	std::map<std::string, Client*>::iterator it;

	for (it = nick_clients_.begin(); it != nick_clients_.end(); it++)
	{
		if (it->first == nick)
			return (true);
	}
	return (false);
}

bool	ClientManager::isValidNickname(std::string& nick)
{
	if (nick.length() > 9 || nick.length() == 0)
		return (false);
	if (nick[0] == '#' || nick[0] == '$')
		return (false);
	for (std::string::iterator it = nick.begin(); it != nick.end(); it++)
	{
		if (!std::isprint(*it) || *it == ',')
			return (false);
	}
	
	return (true);
}

bool	ClientManager::isUsedNickname(const std::string& nick)
{
	if (nick_clients_.find(nick) == nick_clients_.end())
		return (false);
	return true;
}

void	ClientManager::addNickClient(const std::string& nick, Client* client)
{
	nick_clients_.insert(std::make_pair(nick, client));
}

void	ClientManager::removeNickClient(const std::string& nick)
{
	nick_clients_.erase(nick);
}

void	ClientManager::addChannelToClient(const std::string& nick, const std::string& channel)
{
	if (nick_clients_.find(nick) == nick_clients_.end())
		return ;
	nick_clients_.find(nick)->second->addChannel(channel);
}

void	ClientManager::removeChannelFromClient(const std::string& nick, const std::string& channel)
{
	if (nick_clients_.find(nick) == nick_clients_.end())
		return ;
	nick_clients_.find(nick)->second->deleteChannel(channel);
}

void	ClientManager::removeClient(int socket)
{
	Client	*temp;

	temp = getClientBySocket(socket);
	if (temp == NULL)
		return ;
	removeNickClient(temp->getNickName());
	socket_clients_.erase(socket);
	temp->removeClient();
	
	delete temp;

	std::cout << "[INFO] ClientManager::removeClient : " << socket << " is removed." << std::endl;
}