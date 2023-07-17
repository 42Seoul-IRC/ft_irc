#ifndef PACKET_MANAGER_HPP
# define PACKET_MANAGER_HPP

#include <map>

#include "../message/Message.hpp"
#include "../client/ClientManager.hpp"
#include "../channel/ChannelManager.hpp"

class PacketManager {
	private:
		char* password_;
		ClientManager client_manager_;
		ChannelManager channel_manager_;
		typedef void (PacketManager::*RecvPacketFunction)(Packet);
		std::map<std::string, RecvPacketFunction> recv_function_map_;
	
	public:
		void init(char *password);
};

#endif