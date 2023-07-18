#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#include <deque>
#include <sys/event.h>

#include "../socket/Socket.hpp"
#include "../packet/PacketManager.hpp"
#include "../packet/Packet.hpp"

class Server {
	private:
		Socket server_socket_;
		PacketManager packet_manager_;
		std::deque<Packet> packet_queue;
		
		int kqueue_;

		void process(void);
		void addSocket(int socket);

	public:
		Server();
		void init(char* port, char* password);
		void run(void);
		void successHandler(int socket);
		void errorHandler(int socket);
};

#endif