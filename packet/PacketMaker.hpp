#ifndef PACKETMAKER_HPP
#define PACKETMAKER_HPP

#include "PacketManager.hpp"
#include "macro.h"

class PacketMaker : public PacketManager
{
	public:
		void ErrAlreadyRegistred(struct Packet& packet);
};

#endif