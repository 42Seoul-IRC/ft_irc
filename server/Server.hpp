#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#include <deque>

#include "../socket/Socket.hpp"
#include "../reactor/Reactor.hpp"
#include "../packet/PacketManger.hpp"
#include "../packet/Packet.hpp"

class Server {
	private:
		Socket server_socket_;
		Reactor socket_reactor_;
		PacketManager packet_manager_;
		std::deque<Packet> packet_queue;
		
		int kqueue_;
		struct kevent* event_;

		void process(void);
		void addSocket(int socket);

	public:
		void init(char* port, char* password);
		void run(void);
		void successHandler(int socket);
		void errorHandler(int socket);
};

#endif