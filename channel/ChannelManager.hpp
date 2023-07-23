#ifndef CHANNEL_MANAGER_HPP
# define CHANNEL_MANAGER_HPP

#include <map>
#include <string>

#include "Channel.hpp"

class ChannelManager {
	public:
		std::map<std::string, Channel*> channels_;

		~ChannelManager();

		void	createChannelByName(const std::string& channel_name);
		void	addClientToChannel(const std::string& channel_name, const std::string& client_name);
		Channel	*getChannelByName(const std::string& channel_name);
		void	deleteChannelByName(const std::string& channel_name);
		void	deleteClientFromChannel(const std::string& channel_name, const std::string& client_name);

		int		getChannelOnClientCount(const std::string& channel_name);
		std::vector<Channel *>	getChannelsByClientName(const std::string& clientName);
		
		bool 	checkClientIsOperator(const std::string& channel_name, const std::string& clientName);
		bool 	checkClientIsInvited(const std::string& channel_name, const std::string& clientName);
		bool 	checkClientIsInChannel(const std::string& channel_name, const std::string& clientName);
		bool	checkChannelPassword(const std::string& channel_name, const std::string& password);

};

#endif