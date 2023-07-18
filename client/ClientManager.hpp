#ifndef CLIENT_MANAGER_HPP
# define CLIENT_MANAGER_HPP

#include <list>
#include <map>

class Client;

class ClientManager {
	private:
		std::map<int, Client*> client_pool_;
		std::map<std::string, Client*> clients_;
	public:
		void	addClientBySocket(int socket); // new client
		Client	*getClientBySocket(int socket);
};

#endif