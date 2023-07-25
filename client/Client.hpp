#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <set>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>

class Client {
	private:
		int client_socket_;
		std::string nickname_;
		std::string username_;
		std::string hostname_;
		std::string servername_;
		std::string realname_;
		std::set<std::string> channels_;
		bool is_pass_;
		bool is_authenticated_;

	public:
		Client(int socket);
		~Client();
		
		std::stringstream buffer_;

		int	getSocket(void) const;
		std::string	getNickName(void);
		const std::string&	getUserName(void) const;
		const std::string&	getHostName(void) const;
		const std::string&	getServerName(void) const;
		const std::string&	getRealName(void) const;
		std::set<std::string>& getChannels(void);
		bool getIsAuthenticated(void) const;
		bool getIsPass(void) const;
		std::string getHost(void);

		void setIsPass(bool is_pass);
		void setIsAuthenticated(bool is_authenticated);
		void setNickName(const std::string& nickname);
		void setUserName(const std::string& username);
		void setHostName(const std::string& hostname);
		void setServerName(const std::string& servername);
		void setRealName(const std::string& realname);
		void addChannel(const std::string& channel);
		void deleteChannel(const std::string& channel);

		void removeClient(void);

		//bool client exists
};

#endif