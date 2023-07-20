#ifndef PACKET_MANAGER_HPP
# define PACKET_MANAGER_HPP

#include <sys/socket.h>
#include <map>

#include "../message/Message.hpp"
#include "../packet/Packet.hpp"
#include "../client/ClientManager.hpp"
#include "../channel/ChannelManager.hpp"

class PacketManager {
	private:
		char* password_;
		typedef void (PacketManager::*RecvPacketFunction)(struct Packet&);
		std::map<std::string, RecvPacketFunction> recv_function_map_;
		ChannelManager channel_manager_;
	
	public:
		ClientManager client_manager_;
		
		void init(char *password);
		void execute(struct Packet packet);
		void removeClientBySocket(int socket);
		std::string getNickBySocket(int socket);

		void sendPacket(struct Packet& packet);
		void sendPacket(Message message, Channel *channel);
		void sendPacket(Message message, Channel *channel, std::string exclude_nick);

		void	pass(struct Packet& packet);
		void	nick(struct Packet& packet);
		void	user(struct Packet& packet);
		void	privmsg(struct Packet& packet);
		void	ping(struct Packet& packet);
		void	quit(struct Packet& packet);

		void	join(struct Packet& packet);
		void	part(struct Packet& packet);
		void	kick(struct Packet& packet);

		void	invite(struct Packet& packet);
		void	topic(struct Packet& packet);
		void	mode(struct Packet& packet);
};

#endif