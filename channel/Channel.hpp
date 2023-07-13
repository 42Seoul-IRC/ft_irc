#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>

#include "../client/Client.hpp"

class Channel {
	private:
		std::string channel_name_;
		std::string topic_;
		std::string password_;
		std::vector<std::string> operators_;
		std::map<std::string, Client*> clients_;
		std::vector<std::string> invited_clients_;
		int limit_;
		int mode_;
};

#endif