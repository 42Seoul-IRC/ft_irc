#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#include <deque>

#include "../socket/Socket.hpp"
#include "../reactor/Reactor.hpp"
#include "../session/Session.hpp"
#include "../session/SessionManager.hpp"
#include "../packet/PacketManger.hpp"
#include "../packet/Packet.hpp"

class Server {
	private:
		Socket server_socket_;
		Reactor socket_reactor_;
		SessionManager session_manager_;
		PacketManager packet_manager_;

		std::deque<Packet> packet_queue;

		void successHandler(int socket);
		void errorHandler(int socket);
		void process(void);

	public:
		void init(char* port, char* password);
		void start(void);
};

#endif