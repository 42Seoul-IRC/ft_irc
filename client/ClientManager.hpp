#ifndef CLIENT_MANAGER_HPP
# define CLIENT_MANAGER_HPP

#include <list>
#include <map>
#include "Client.hpp"

class ClientManager {
	public:
		std::map<int, Client*> socket_clients_;
		std::map<std::string, Client*> nick_clients_;
		
		void	addClientBySocket(int socket);
		Client	*getClientBySocket(int socket);
};

#endif