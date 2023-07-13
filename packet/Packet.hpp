#ifndef PACKET_HPP
#define PACKET_HPP

#include "../message/Message.hpp"

struct Packet {
	int session_index;
	Message message;
};

#endif