#ifndef CLIENT_MANAGER_HPP
# define CLIENT_MANAGER_HPP

#include <vector>
#include <map>

class Client;

class ClientManager {
	private:
		std::vector<Client*> client_pool_;
		std::map<std::string, Client*> clients_;
};

#endif