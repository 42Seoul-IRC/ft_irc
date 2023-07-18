#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <set>

class Client {
	private:
		int client_socket_;
		std::string nickname_;
		std::string username_;
		std::string hostname_;
		std::set<std::string> channels_;
		// bool is_authenticated_;
	public:
		Client(int socket);
		std::string	getNickName();
		std::set<std::string> getChannels();
};

#endif