#ifndef PACKET_HPP
#define PACKET_HPP

#include "../message/Message.hpp"

struct Packet 
{
	int client_socket;
	Message message;
};

#endif