#include "PacketMaker.hpp"

void PacketMaker::Err_alreadyregistred(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix("irc.local");
	message.setCommand(ERR_ALREADYREGISTERED);
	message.addParam(client->getHostName());
	message.setTrailing("You may not register");

	struct Packet packet = {client->getSocket(), message};
	sendPacket(packet);
}