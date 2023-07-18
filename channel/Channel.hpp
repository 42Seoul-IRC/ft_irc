#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>

#include "../client/Client.hpp"
#include "ChannelMode.hpp"

class Channel {
	public:
		std::string channel_name_;
		std::string topic_;
		std::string password_;
		std::set<std::string> operators_;
		std::set<std::string> clients_; // 필요한  경우, 바로 map<std::string, *client> 사용할 것!
		std::set<std::string> invited_clients_;
		int limit_;
		int mode_;
};

#endif