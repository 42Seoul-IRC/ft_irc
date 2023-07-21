#ifndef PACKET_MANAGER_HPP
# define PACKET_MANAGER_HPP

#include <sys/socket.h>
#include <map>

#include "../message/Message.hpp"
#include "../client/ClientManager.hpp"
#include "../channel/ChannelManager.hpp"
#include "Packet.hpp"
#include "PacketMaker.hpp"

class PacketMaker;

class PacketManager
{
	protected:
		char* password_;
		typedef void (PacketManager::*RecvPacketFunction)(struct Packet&);
		std::map<std::string, RecvPacketFunction> recv_function_map_;
		
	public:
		PacketManager();
		~PacketManager();

		ChannelManager channel_manager_;
		ClientManager client_manager_;
		PacketMaker* packet_maker_;
		std::string valid_channel_modes_;
		
		void init(char *password);
		void execute(struct Packet& packet);
		void removeClientBySocket(int socket);
		std::string getNickBySocket(int socket);

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

		// void	printPacket(struct Packet& packet);
		void	mode(struct Packet& packet);

		void	channel(struct Packet& packet);
		void	client(struct Packet& packet);
};

#endif