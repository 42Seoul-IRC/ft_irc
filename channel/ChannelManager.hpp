#ifndef CHANNEL_MANAGER_HPP
# define CHANNEL_MANAGER_HPP

#include <map>
#include <string>

class Channel;

class ChannelManager {
	private:
		std::map<std::string, Channel*> channels_;
};

#endif