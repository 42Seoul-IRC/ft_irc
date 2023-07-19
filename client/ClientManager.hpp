#ifndef CLIENT_MANAGER_HPP
#define CLIENT_MANAGER_HPP

#define FIRST_CHARACTER "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

#include <list>
#include <map>
#include "Client.hpp"

class ClientManager
{
public:
	std::map<int, Client *> socket_clients_;
	std::map<std::string, Client *> nick_clients_;

	void addClientBySocket(int socket);
	Client *getClientBySocket(int socket);
	Client *getClientByNick(std::string nick);

	bool clientIsAuth(int socket);
	bool clientIsInServer(std::string nick);
	bool isValidNickname(std::string nick);
	bool isUsedNickname(std::string nick);

	void addNickClient(std::string nick, Client *client);
	void removeNickClient(std::string nick);

	void addChannelToClient(std::string nick, std::string channel);
	void removeChannelFromClient(std::string nick, std::string channel);
};

#endif