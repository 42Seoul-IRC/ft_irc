#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <set>
#include <arpa/inet.h>

class Client {
	private:
		int client_socket_;
		std::string nickname_;
		std::string username_;
		std::string hostname_;
		std::set<std::string> channels_;
		bool is_authenticated_;

	public:
		Client(int socket);
<<<<<<< HEAD
		
		const int	getSocket(void) const;
		const std::string&	getNickName(void) const;
		const std::string&	getUserName(void) const;
		const std::string&	getHostName(void) const;
		std::set<std::string>& getChannels(void);
		bool getIsAuthenticated(void) const;
		std::string getHost(void) const;

		void setNickName(const std::string& nickname);
		void setUserName(const std::string& username);
		void setHostName(const std::string& hostname);
		void addChannel(const std::string& channel);
		void deleteChannel(const std::string& channel);

		void setIsAuthenticated(bool is_authenticated);

		//bool client exists
=======

		int			getSocket();
		std::string	getNickName();
		std::string	getUserName();
		std::string	getHostName();
		std::set<std::string> getChannels();

>>>>>>> 95007fd22122f80160a7f867ae0e77431d834e97
};

#endif