#ifndef PACKET_MANAGER_HPP
# define PACKET_MANAGER_HPP

#include <map>

#include "../message/Message.hpp"
#include "../packet/Packet.hpp"
#include "../client/ClientManager.hpp"
#include "../channel/ChannelManager.hpp"

class PacketManager {
	private:
		char* password_;
		typedef void (PacketManager::*RecvPacketFunction)(struct Packet);
		std::map<std::string, RecvPacketFunction> recv_function_map_;
		ChannelManager channel_manager_;
	
	public:
		ClientManager client_manager_;
		
		void init(char *password);
		void execute(struct Packet packet);
		void removeClientBySocket(int socket);
};

#endif