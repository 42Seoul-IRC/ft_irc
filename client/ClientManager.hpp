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
	Client *getClientByNick(const std::string& nick);

	bool clientIsAuth(int socket);
	bool clientIsInServer(const std::string& nick);
	bool isValidNickname(std::string& nick);
	bool isUsedNickname(const std::string& nick);

	void addNickClient(const std::string& nick, Client *client);
	void removeNickClient(const std::string& nick);

	void addChannelToClient(const std::string& nick, const std::string& channel);
	void removeChannelFromClient(const std::string& nick, const std::string& channel);
	void removeClient(int socket);
};

#endif