#ifndef CLIENT_MANAGER_HPP
#define CLIENT_MANAGER_HPP

#include <list>
#include <map>
#include "Client.hpp"

class ClientManager
{
public:
	std::map<int, Client *> socket_clients_;
	std::map<std::string, Client *> nick_clients_;

	~ClientManager();

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
	void removeClient(int socket);
};

#endif