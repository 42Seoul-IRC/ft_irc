#include "PacketManager.hpp"

void PacketManager::init(char *password)
{
	password_ = password;

	recv_function_map_["PASS"] = &PacketManager::pass;
	recv_function_map_["NICK"] = &PacketManager::nick;
	recv_function_map_["USER"] = &PacketManager::user;
	recv_function_map_["JOIN"] = &PacketManager::join;
	recv_function_map_["PRIVMSG"] = &PacketManager::privmsg;
	recv_function_map_["PART"] = &PacketManager::part;
	recv_function_map_["PING"] = &PacketManager::ping;
	recv_function_map_["QUIT"] = &PacketManager::quit;
	recv_function_map_["KICK"] = &PacketManager::kick;
	recv_function_map_["INVITE"] = &PacketManager::invite;
	recv_function_map_["TOPIC"] = &PacketManager::topic;
	recv_function_map_["MODE"] = &PacketManager::mode;
}

void	PacketManager::removeClientBySocket(int socket)
{
	//remove socket in client manager
	std::string nick = client_manager_.socket_clients_[socket]->getNickName();
	std::set<std::string> channel_included = client_manager_.socket_clients_[socket]->getChannels();
	for (std::set<std::string>::iterator it = channel_included.begin(); it != channel_included.end(); it++)
	{
		Channel* channel = channel_manager_.channels_[*it];
		if (channel)
		{
			channel->operators_.erase(nick);
			channel->clients_.erase(nick);
			channel->invited_clients_.erase(nick);
		}
	}
	
	client_manager_.socket_clients_.erase(socket);
	client_manager_.nick_clients_.erase(nick);
}

void PacketManager::execute(struct Packet packet)
{
	std::map<std::string, RecvPacketFunction>::iterator it = recv_function_map_.find(packet.message.command_);
	if (it != recv_function_map_.end())
	{
		(this->*(it->second))(packet);
	}
}
