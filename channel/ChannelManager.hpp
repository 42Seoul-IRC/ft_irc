#ifndef CHANNEL_MANAGER_HPP
# define CHANNEL_MANAGER_HPP

#include <map>
#include <string>

#include "Channel.hpp"

class ChannelManager {
	public:
		std::map<std::string, Channel*> channels_;
};

#endif