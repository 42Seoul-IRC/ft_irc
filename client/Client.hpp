#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <set>
#include <unistd.h>
#include <arpa/inet.h>

class Client {
	private:
		int client_socket_;
		std::string nickname_;
		std::string username_;
		std::string hostname_;
		std::set<std::string> channels_;
		bool is_pass_;
		bool is_authenticated_;

	public:
		Client(int socket);
		
		int	getSocket(void) const;
		std::string	getNickName(void);
		const std::string&	getUserName(void) const;
		const std::string&	getHostName(void) const;
		std::set<std::string>& getChannels(void);
		bool getIsAuthenticated(void) const;
		bool getIsPass(void) const;
		std::string getHost(void);

		void setIsPass(bool is_pass);
		void setIsAuthenticated(bool is_authenticated);
		void setNickName(const std::string& nickname);
		void setUserName(const std::string& username);
		void setHostName(const std::string& hostname);
		void addChannel(const std::string& channel);
		void deleteChannel(const std::string& channel);

		void removeClient(void);

		//bool client exists
};

#endif