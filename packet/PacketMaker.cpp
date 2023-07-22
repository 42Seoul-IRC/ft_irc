#include "PacketMaker.hpp"

PacketMaker::PacketMaker(ClientManager& client_manager_, ChannelManager& channel_manager_) : client_manager_(client_manager_), channel_manager_(channel_manager_)
{

}

PacketMaker::~PacketMaker()
{
	
}

void PacketMaker::log(struct Packet &packet)
{
	std::cout << "[LOG] {irc.webserv -> " << packet.client_socket << "} - " << packet.message.toString();
}

void PacketMaker::sendPacket(struct Packet& packet)
{
	log(packet);
	::send(packet.client_socket, packet.message.toString().c_str(), packet.message.toString().length(), 0);
}

void PacketMaker::sendPacket(Message message, std::string channel_name)
{
	Channel *channel = channel_manager_.getChannelByName(channel_name);
	if (!channel)
		return ;
	for (std::set<std::string>::iterator it = channel->clients_.begin(); it != channel->clients_.end(); it++)
	{
		int socket = client_manager_.nick_clients_[*it]->getSocket();

		Packet packet = {
			.client_socket = socket,
			.message = message
		};
		sendPacket(packet);
	}
}

void PacketMaker::sendPacket(Message message, std::string channel_name, std::string exclude_nick)
{
	Channel *channel = channel_manager_.getChannelByName(channel_name);
	if (!channel)
		return ;
	for (std::set<std::string>::iterator it = channel->clients_.begin(); it != channel->clients_.end(); it++)
	{
		if (*it == exclude_nick)
			continue;

		int socket = client_manager_.nick_clients_[*it]->getSocket();

		Packet packet = {
			.client_socket = socket,
			.message = message
		};
		sendPacket(packet);
	}
}

// Connection
void PacketMaker::CapLs(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand("CAP");
	message.addParam("*");
	message.addParam("LS");

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

// Common Error
void PacketMaker::ErrNotRegistered(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_NOTREGISTERED);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_NOTREGISTERED_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrNeedMoreParams(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_NEEDMOREPARAMS);
	message.addParam(client->getNickName());
	message.addParam(packet.message.getCommand());
	message.setTrailing(ERR_NEEDMOREPARAMS_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrNoSuchChannel(struct Packet& packet, std::string channel_name)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_NOSUCHCHANNEL);
	message.addParam(client->getNickName());
	message.addParam(channel_name);
	message.setTrailing(ERR_NOSUCHCHANNEL_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrNoSuchChannel(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_NOSUCHCHANNEL);
	message.addParam(client->getNickName());
	message.addParam(packet.message.getParams()[0]);
	message.setTrailing(ERR_NOSUCHCHANNEL_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrNoSuchNick(struct Packet& packet, std::string nick_name)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_NOSUCHNICK);
	message.addParam(client->getNickName());
	message.addParam(nick_name);
	message.setTrailing(ERR_NOSUCHNICK_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrNoSuchNick(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	std::string nick_name = packet.message.getParams()[0];

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_NOSUCHNICK);
	message.addParam(client->getNickName());
	message.addParam(nick_name);
	message.setTrailing(ERR_NOSUCHNICK_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrNotOnChannel(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_NOTONCHANNEL);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_NOTONCHANNEL_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);	
}

void PacketMaker::ErrChanOPrivsNeeded(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_CHANOPRIVSNEEDED);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_CHANOPRIVSNEEDED_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);	
}

// RPL
void PacketMaker::RplWelcome(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setCommand(RPL_WELCOME);
	message.addParam(client->getNickName());
	message.setTrailing(RPL_WELCOME_MSG + client->getHost());

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::RplNoTopic(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setCommand(RPL_NOTOPIC);
	message.addParam(client->getHostName());
	message.addParam(client->getNickName());
	message.addParam(packet.message.getParams()[0]);
	message.setTrailing(RPL_NOTOPIC_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::RplTopic(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(RPL_TOPIC);
	message.addParam(client->getNickName());
	message.addParam(packet.message.getCommand());
	message.setTrailing(packet.message.getTrailing());

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);	
}

void PacketMaker::RplTopicWhoTime(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	std::string channel_name = packet.message.getParams()[0];
	Channel *channel = channel_manager_.getChannelByName(channel_name);

	message.setCommand(RPL_TOPICWHOTIME);
	message.addParam(client->getHostName());
	message.addParam(client->getNickName());
	message.addParam(channel_name);
	message.addParam(channel->getTopicSetter());

	std::stringstream	ss(channel->getTopicSetTime());
	std::string time_str;

	ss >>time_str;	
	message.setTrailing(time_str);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);	
}

void PacketMaker::RplNamReply(struct Packet& packet)
{
	Message message;
	std::string channel_name = packet.message.getTrailing();
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
  
	message.setPrefix(SERVER_NAME);
	message.setCommand(RPL_NAMREPLY);
	message.addParam(client->getNickName());
	message.addParam("=");
	message.addParam(channel_name);
	message.setTrailing(channel_manager_.getChannelByName(channel_name)->getClientsString());

	sendPacket(message, channel_name);
}

void PacketMaker::RplEndOfNames(struct Packet& packet)
{
	Message message;
	std::string channel_name = packet.message.getTrailing();
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(RPL_ENDOFNAMES);
	message.addParam(client->getNickName());
	message.addParam(channel_name);
	message.setTrailing(RPL_ENDOFNAMES_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);	
}

void PacketMaker::RplInviting(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	std::string target_nick = packet.message.getParams()[0];
	std::string channel_name = packet.message.getParams()[1];

	message.setCommand(RPL_INVITING);
	message.addParam(client->getHostName());
	message.addParam(client->getNickName());
	message.addParam(target_nick);
	message.addParam(channel_name);
	message.setTrailing("Inviting " + target_nick + " to " + channel_name);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

// PASS
void PacketMaker::ErrAlreadyRegistered(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_ALREADYREGISTERED);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_ALREADYREGISTERED_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrPasswdMismatch(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_PASSWDMISMATCH);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_PASSWDMISMATCH_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

//NICK
void PacketMaker::ErrNoNicknameGiven(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_NONICKNAMEGIVEN);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_NONICKNAMEGIVEN_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrErroneusNickname(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_ERRONEUSNICKNAME);
	message.addParam(client->getNickName());
	message.addParam(packet.message.getParams()[0]);
	message.setTrailing(ERR_ERRONEUSNICKNAME_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrNicknameInUse(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_NICKNAMEINUSE);
	message.addParam(client->getNickName());
	message.addParam(packet.message.getParams()[0]);
	message.setTrailing(ERR_NICKNAMEINUSE_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

Message PacketMaker::NickSuccess(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(client->getHost());
	message.setCommand("NICK");
	message.setTrailing(packet.message.getParams()[0]);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);

	return message;
}

// USER
void PacketMaker::ErrAlreadyRegistred(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_ALREADYREGISTERED);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_ALREADYREGISTERED_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

// PRIVMSG
void PacketMaker::ErrNoRecipient(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_NORECIPIENT);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_NORECIPIENT_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrNoTextToSend(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_NOTEXTTOSEND);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_NOTEXTTOSEND_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrCannotSendToChan(struct Packet& packet, std::string channel_name)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_CANNOTSENDTOCHAN);
	message.addParam(client->getNickName());
	message.addParam(channel_name);
	message.setTrailing(ERR_CANNOTSENDTOCHAN_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::PrivmsgToChannel(struct Packet& packet, std::string target_channel)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(client->getHost());
	message.setCommand("PRIVMSG");
	message.addParam(target_channel);
	message.setTrailing(packet.message.getTrailing());

	sendPacket(message, target_channel, client->getNickName());
}

void PacketMaker::PrivmsgToUser(struct Packet& packet, std::string target_nick)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(client->getHost());
	message.setCommand("PRIVMSG");
	message.addParam(target_nick);
	message.setTrailing(packet.message.getTrailing());

	struct Packet pkt = {client_manager_.getClientByNick(target_nick)->getSocket(), message};
	sendPacket(pkt);
}

// QUIT
Message PacketMaker::Quit(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(client->getHost());
	message.setCommand("QUIT");
	message.setTrailing(packet.message.getTrailing());

	return message;
}

// PING
Message PacketMaker::Ping(struct Packet& packet)
{
	Message message;

	message.setCommand("PONG");
	message.addParam(SERVER_NAME);
	message.addParam(packet.message.getParams()[0]);

	return message;
}

// JOIN
void PacketMaker::ErrBadChannelKey(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_BADCHANNELKEY);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_BADCHANNELKEY_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void PacketMaker::ErrInviteOnlyChan(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_INVITEONLYCHAN);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_INVITEONLYCHAN_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);	
}

void PacketMaker::ErrChannelIsFull(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_CHANNELISFULL);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_CHANNELISFULL_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);	
}

void PacketMaker::BroadcastJoin(struct Packet& packet)
{
	Message message;

	message.setPrefix(client_manager_.getClientBySocket(packet.client_socket)->getHost());
	message.setCommand("JOIN");
	message.setTrailing(packet.message.getTrailing());

	sendPacket(message, packet.message.getTrailing());
}

void PacketMaker::Broadcast(struct Packet& packet, std::string cmd)
{
	Message message;

	message.setPrefix(client_manager_.getClientBySocket(packet.client_socket)->getHost());
	message.setCommand(cmd);
	message.setTrailing(packet.message.getTrailing());

	sendPacket(message, packet.message.getTrailing());
}

void PacketMaker::ErrBadChanMask(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	std::string channel_name = packet.message.getParams()[0];

	message.setPrefix(SERVER_NAME);
	message.setCommand(ERR_BADCHANMASK);
	message.addParam(channel_name);
	message.setTrailing(ERR_BADCHANMASK_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

// PART
void PacketMaker::BroadcastPart(struct Packet& packet)
{
	Message message;

	message.setPrefix(client_manager_.getClientBySocket(packet.client_socket)->getHost());
	message.setCommand("PART");
	message.addParam(packet.message.getCommand());
	message.setTrailing(packet.message.getTrailing());

	sendPacket(message, packet.message.getCommand());
}

void PacketMaker::ErrUserNotInChannel(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	message.setPrefix(client_manager_.getClientBySocket(packet.client_socket)->getHost());
	message.setCommand(ERR_USERNOTINCHANNEL);
	message.addParam(client->getNickName());
	message.setTrailing(ERR_USERNOTINCHANNEL_MSG);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

// KICK
void PacketMaker::BroadcastKick(struct Packet& packet)
{
	Message message;
	std::string channel_name(packet.message.getPrefix());

	message.setPrefix(client_manager_.getClientBySocket(packet.client_socket)->getHost());
	message.setCommand("KICK");
	message.addParam(channel_name);
	message.addParam(packet.message.getCommand());
	message.setTrailing(packet.message.getTrailing());

	sendPacket(message, channel_name);
}

// INVITE
void PacketMaker::ErrUserOnChannel(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	std::string target_nick = packet.message.getParams()[0];
	std::string channel_name = packet.message.getParams()[1];

	message.setCommand(ERR_USERONCHANNEL);
	message.addParam(client->getHostName());
	message.addParam(client->getNickName());
	message.addParam(target_nick);
	message.addParam(channel_name);
	message.setTrailing("is alreay on channel");

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

//MODE
//RPL_CHANNELMODEIS (324)

void	PacketMaker::RplChannelModeIs(struct Packet& packet, Channel *channel)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	std::string channel_name = channel->getChannelName();
	std::string channel_mode = channel->getChannelModeString();

	message.setPrefix(SERVER_NAME);
	message.setCommand("324");
	message.addParam(client->getNickName());
	message.addParam(channel_name);
	message.setTrailing(channel_mode);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
} 


// RPL_CREATIONTIME (329)

void	PacketMaker::RplCreationTime(struct Packet& packet, Channel *channel)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	std::string channel_name = channel->getChannelName();
	std::string channel_created_time = std::to_string(channel->getChannelCreatedTime());

	message.setPrefix(SERVER_NAME);
	message.setCommand("329");
	message.addParam(client->getNickName());
	message.addParam(channel_name);
	message.setTrailing(channel_created_time);

	struct Packet pkt = {client->getSocket(), message};
	sendPacket(pkt);
}

void	PacketMaker::BroadcastMode(Channel *channel, std::string changed_mode_buffer, std::string param_buffer)
{
	Message message;
	std::string channel_name = channel->getChannelName();

	message.setPrefix(SERVER_NAME);
	message.setCommand("MODE");
	message.addParam(channel_name);
	message.addParam(changed_mode_buffer);
	message.addParam(param_buffer);

}