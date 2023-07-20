#include "PacketManager.hpp"
#include "macro.h"

void	PacketManager::pass(struct Packet& packet)
{
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	if (client->getIsAuthenticated())
	{
		message.setCommand(ERR_ALREADYREGISTERED);
		message.addParam(client->getHostName());
		message.setTrailing("You may not register");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	if (packet.message.getParams().size() != 1)
	{
		message.setCommand(ERR_NEEDMOREPARAMS);
		message.addParam(client->getHostName());
		message.addParam(packet.message.getCommand());
		message.setTrailing("Not enough parameters");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	if (packet.message.getParams()[0] != password_)
	{
		message.setCommand(ERR_PASSWDMISMATCH);
		message.addParam(client->getHostName());
		message.setTrailing("Password incorrect");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	client->setIsAuthenticated(true);
}

void	PacketManager::nick(struct Packet& packet)
{
	Message message;

	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	if (!client->getIsAuthenticated())
	{
		message.setCommand(ERR_NOTREGISTERED);
		message.addParam(client->getHostName());
		message.setTrailing("You have not registered");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	if (packet.message.getParams().size() != 1)
	{
		message.setCommand(ERR_NONICKNAMEGIVEN);
		message.addParam(client->getHostName());
		message.setTrailing("No nickname given");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	std::string old_nick = client->getNickName();
	std::string new_nick = packet.message.getParams()[0];
	if (!client_manager_.isValidNickname(new_nick))
	{
		message.setCommand(ERR_ERRONEUSNICKNAME);
		message.addParam(client->getHostName());
		message.addParam(new_nick);
		message.setTrailing("Erroneous nickname");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	if (!client_manager_.isUsedNickname(new_nick))
	{
		message.setCommand(ERR_NICKNAMEINUSE);
		message.addParam(client->getHostName());
		message.addParam(new_nick);
		message.setTrailing("Nickname is already in use");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	Client *nick_client = client_manager_.getClientByNick(new_nick);
	if (!nick_client)
	{
		client_manager_.addNickClient(new_nick, client);
		client->setNickName(new_nick);

		message.setCommand(RPL_WELCOME);
		message.addParam(client->getHostName());
		message.setTrailing("Welcome to the Internet Relay Network " + new_nick + "!" + client->getHostName() + "@" + client->getHost());

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);
	}
	else
	{
		message.setPrefix(old_nick);
		message.setCommand("NICK");
		message.addParam(new_nick);

		client_manager_.removeNickClient(old_nick);
		client_manager_.addNickClient(new_nick, client);

		std::vector<Channel *> channels = channel_manager_.getChannelsByClientName(old_nick);
		for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
			(*it)->changeClientInfo(old_nick, new_nick);

			sendPacket(message, (*it));
		}
	}
}

void	PacketManager::user(struct Packet& packet)
{
	Message message;

	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	if (!client->getIsAuthenticated())
	{
		message.setCommand(ERR_NOTREGISTERED);
		message.addParam(client->getHostName());
		message.setTrailing("You have not registered");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	if (packet.message.getParams().size() != 4)
	{
		message.setCommand(ERR_NEEDMOREPARAMS);
		message.addParam(client->getHostName());
		message.addParam(packet.message.getCommand());
		message.setTrailing("Not enough parameters");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	if (client->getUserName() != "")
	{
		message.setCommand(ERR_ALREADYREGISTERED);
		message.addParam(client->getHostName());
		message.setTrailing("You may not register");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	client->setUserName(packet.message.getParams()[0]);
	client->setHostName(packet.message.getParams()[3]);
}

void	PacketManager::privmsg(struct Packet& packet)
{
	Message message;

	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	if (!client->getIsAuthenticated())
	{
		message.setCommand(ERR_NOTREGISTERED);
		message.addParam(client->getHostName());
		message.setTrailing("You have not registered");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	if (packet.message.getParams().size() != 1)
	{
		message.setCommand(ERR_NORECIPIENT);
		message.addParam(client->getHostName());
		message.setTrailing("No recipient given" + packet.message.getCommand());

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	if (packet.message.getTrailing() == "")
	{
		message.setCommand(ERR_NOTEXTTOSEND);
		message.addParam(client->getHostName());
		message.setTrailing("No text to send");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}

	if (packet.message.getParams()[0][0] == '#')
	{
		Channel *channel = channel_manager_.getChannelByName(packet.message.getParams()[0]);
		if (!channel)
		{
			message.setCommand(ERR_NOSUCHCHANNEL);
			message.addParam(client->getHostName());
			message.addParam(packet.message.getParams()[0]);
			message.setTrailing("No such channel");

			struct Packet packet = {client->getSocket(), message};
			sendPacket(packet);

			return ;
		}

		if (!channel_manager_.checkClientIsInChannel(packet.message.getParams()[0], client->getNickName()))
		{
			message.setCommand(ERR_CANNOTSENDTOCHAN);
			message.addParam(client->getHostName());
			message.addParam(packet.message.getParams()[0]);
			message.setTrailing("Cannot send to channel");

			struct Packet packet = {client->getSocket(), message};
			sendPacket(packet);

			return ;
		}

		message.setPrefix(client->getNickName());
		message.setCommand("PRIVMSG");
		message.addParam(packet.message.getParams()[0]);
		message.setTrailing(packet.message.getTrailing());

		sendPacket(message, channel);
	}
	else
	{
		Client *target_client = client_manager_.getClientByNick(packet.message.getParams()[0]);
		if (!target_client)
		{
			message.setCommand(ERR_NOSUCHNICK);
			message.addParam(client->getHostName());
			message.addParam(packet.message.getParams()[0]);
			message.setTrailing("No such nick");

			struct Packet packet = {client->getSocket(), message};
			sendPacket(packet);

			return ;
		}

		message.setPrefix(client->getNickName());
		message.setCommand("PRIVMSG");
		message.addParam(packet.message.getParams()[0]);
		message.setTrailing(packet.message.getTrailing());

		struct Packet packet = {target_client->getSocket(), message};
		sendPacket(packet);
	}
}

void	PacketManager::quit(struct Packet& packet)
{
	
}

void	PacketManager::ping(struct Packet& packet)
{
	
}


void	PacketManager::join(struct Packet& packet)
{
/* // 패킷 만드는 부분 수정 필요할 듯 -> 같은 message에 addParams 호출중
0. 파라미터가 1개 이상인지 -> 461 (ERR_NEEDMOREPARAMS)
1. 해당 채널의 이름이 유효한지 (채널이 없으면 생성해줘야함)
2. 해당 채널에 이미 현재 fd가 접속해 있는지 -> 무시
3. 해당 채널이 잠겨있는지 -> 유효한 비밀번호를 입력하였는지
4. 해당 채널이 초대 전용인지 -> 유저가 초대 리스트에 속해 있는지
5. 해당 채널이 인원 제한이 있는지 -> 인원 수 제한 검사
6. 해당 채널에 유저를 추가
7. 해당 유저에 채널 이름 추가
8. 해당 채널에 토픽이 존재한다면 토픽을 전송 (RPL_TOPIC)
9. 해당 채널의 모든 유저에게 join 메시지 전송
*/
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	if (packet.message.getParams().size() < 1)
	{
		message.setCommand(ERR_NEEDMOREPARAMS);
		message.addParam(client->getHostName());
		message.setTrailing("Not enough parameters");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		return ;
	}
	std::string client_name = client->getNickName();
	std::vector<std::string> channel_names;
	std::vector<std::string> channel_passwords;
	
	if (packet.message.getParams().size() >= 1)
		channel_names = packet.message.getParams()[0]; // need split
	if (packet.message.getParams().size() >= 2)
		channel_passwords = packet.message.getParams()[1]; // need split
	
	std::vector<std::string>::iterator it1 = channel_names.begin();
	std::vector<std::string>::iterator it2 = channel_passwords.begin();
	for (; it1 != channel_names.end(); ++it1, ++it2)
	{
		if (channel_manager_.getChannelByName(*it1) == NULL)
			channel_manager_.createChannelByName(*it1);
		if (channel_manager_.checkClientIsInChannel(*it1, client_name))
			continue ;
		if (channel_manager_.getChannelMode(*it1) & MODE_PASSWORD)
		{
			if (channel_manager_.checkChannelPassword(*it1, *it2) == false)
			{
				message.setCommand(ERR_BADCHANNELKEY);
				message.addParam(client->getHostName());
				message.setTrailing("Cannot join channel (+k)");

				struct Packet packet = {client->getSocket(), message};
				sendPacket(packet);
				
				continue ;
			}
		}
		if (channel_manager_.getChannelMode(*it1) & MODE_INVITE)
		{
			if (channel_manager_.checkClientIsInvited(*it1, client_name) == false)
			{
				message.setCommand(ERR_INVITEONLYCHAN);
				message.addParam(client->getHostName());
				message.setTrailing("Cannot join channel (+i)");

				struct Packet packet = {client->getSocket(), message};
				sendPacket(packet);
				
				continue ;
			}
		}
		if (!(channel_manager_.getChannelByName(*it1)->checkChannelCapacity()))
		{
			message.setCommand(ERR_CHANNELISFULL);
			message.addParam(client->getHostName());
			message.setTrailing("Cannot join channel (+l)");

			struct Packet packet = {client->getSocket(), message};
			sendPacket(packet);
				
			continue ;
		}
		channel_manager_.addClientToChannel(*it1, client_name);
		client_manager_.addChannelToClient(client_name, *it1);
		if (channel_manager_.getChannelByName(*it1)->getChannelMode() & MODE_TOPIC)
		{
			message.setCommand(RPL_TOPIC);
			message.addParam(client->getHostName());
			message.setTrailing(channel_manager_.getChannelByName(*it1)->getTopic());

			struct Packet packet = {client->getSocket(), message};
			sendPacket(packet);
		}
		// broadcast join message
		message.setCommand("JOIN");
		message.addParam(client->getHostName());
		message.setTrailing(*it1);

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);
	}
}

void	PacketManager::part(struct Packet& packet)
{
/*
1. 파라미터가 1개 이상인지 -> 461 (ERR_NEEDMOREPARAMS)
2. 해당 채널이 유효한지 -> 403 (ERR_NOSUCHCHANNEL)
3. 해당 채널에 part를 호출한 유저가 속해 있는지 -> 442 (ERR_NOTONCHANNEL)
4. 해당 채널에 속한 모든 유저에게 part 메시지 전송
5. 해당 채널에서 유저를 제거
6. 유저의 채널 리스트에서 해당 채널의 이름 제거
*/
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	if (packet.message.getParams().size() < 1)
	{
		// ERR_NEEDMOREPARAMS
		return ;
	}
	std::string client_name = client->getNickName();
	std::vector<std::string> channel_names = packet.message.getParams()[0]; //need split
	std::string part_message = packet.message.getTrailing();

	std::vector<std::string>::iterator it = channel_names.begin();
	for (; it != channel_names.end(); ++it)
	{
		if (channel_manager_.getChannelByName(*it) == NULL)
		{
			// ERR_NOSUCHCHANNEL
			continue ;
		}
		if (channel_manager_.checkClientIsInChannel(*it, client_name) == false)
		{
			// ERR_NOTONCHANNEL
			continue ;
		}

		//broadcast part message

		channel_manager_.deleteClientFromChannel(*it, client_name);
		client_manager_.removeChannelFromClient(client_name, *it);
	}
}

void	PacketManager::kick(struct Packet& packet)
{
/*
1. 파라미터가 2개 이상인지 -> 461 (ERR_NEEDMOREPARAMS)
2. 해당 채널이 유효한지 -> 403 (ERR_NOSUCHCHANNEL)
3. 타겟 닉네임이 유효한지 -> 401 (ERR_NOSUCHNICK)
4. 해당 채널에 kick을 호출한 유저가 속해 있는지 -> 442 (ERR_NOTONCHANNEL)
5. kick을 호출한 유저가 대상 채널의 오퍼레이터 인지 -> 482 (ERR_CHANOPRIVSNEEDED)
6. 대상 유저가 해당 채널에 속해 있는지 -> 441 (ERR_USERNOTINCHANNEL)
7. 해당 채널의 모든 유저에게 kick 메시지 전송
8. 해당 채널에서 대상 유저를 제거 -> 채널 객체에서 유저 수 검사 후 채널 삭제
9. 대상 유저의 채널 리스트에서 해당 채널의 이름 제거
*/
	Message message;
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	if (packet.message.getParams().size() < 2)
	{
		// ERR_NEEDMOREPARAMS
		return ;
	}
	std::string client_name = client->getNickName();
	std::string channel_name = packet.message.getParams()[0];
	std::string target_user_name = packet.message.getParams()[1];
	std::string kick_message = packet.message.getTrailing();
	if (channel_manager_.getChannelByName(channel_name) == NULL)
	{
		// ERR_NOSUCHCHANNEL
		return ;
	}
	if (client_manager_.getClientByNick(target_user_name) == NULL)
	{
		// ERR_NOSUCHNICK
		return ;
	}
	if (channel_manager_.checkClientIsInChannel(channel_name, client_name) == false)
	{
		// ERR_NOTONCHANNEL
		return ;
	}
	if (channel_manager_.checkClientIsOperator(channel_name, client_name) == false)
	{
		// ERR_CHANOPRIVSNEEDED
		return ;
	}
	if (channel_manager_.checkClientIsInChannel(channel_name, target_user_name) == false)
	{
		// ERR_USERNOTINCHANNEL
		return ;
	}

	//broadcast kick message

	channel_manager_.deleteClientFromChannel(channel_name, target_user_name);
	client_manager_.removeChannelFromClient(target_user_name, channel_name);
}


void	PacketManager::invite(struct Packet& packet)
{
	//0. Argument check

	Message message;
	
	std::string client_nick = getNickBySocket(packet.client_socket);
	Client * client = client_manager_.getClientByNick(client_nick);

	if (packet.message.getParams().size() != 2 || packet.message.getTrailing().size() != 0 || packet.message.getPrefix().size() != 0)
	{
		// ERR_NEEDMOREPARAMS

		message.setCommand(ERR_NEEDMOREPARAMS);
		message.addParam(client->getHostName());
		message.addParam(client_nick);
		message.addParam(packet.message.getCommand());	
		message.setTrailing("Not enough parameters");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);
		return ;
	}
	
	std::string target_nick = packet.message.getParams()[0];
	std::string channel_name = packet.message.getParams()[1];

	
	//1. validity check

	//chennel exits?
	if (!channel_manager_.getChannelByName(channel_name))
	{
		message.setCommand(ERR_NOSUCHCHANNEL);
		message.addParam(client->getHostName());
		message.addParam(client_nick);
		message.addParam(channel_name);
		message.setTrailing("No such channel");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);
		return ;
	}
	
	//- 명령어를 보낸 사용자가 채널 안에 존재하는가?
	if (!channel_manager_.checkClientIsInChannel(channel_name, client_nick))
	{
		message.setCommand(ERR_NOTONCHANNEL);
		message.addParam(client->getHostName());
		message.addParam(client_nick);
		message.addParam(channel_name);
		message.setTrailing("You're not on that channel");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);
		return ;
	}

	// - invite 모두가 켜져있는 경우에는, operator가 아닌 사람은 초대할 수 없다. 
	if ((channel_manager_.getChannelMode(channel_name) & MODE_INVITE) && !channel_manager_.checkClientIsOperator(channel_name, client_nick))
	{
		message.setCommand(ERR_CHANOPRIVSNEEDED);
		message.addParam(client->getHostName());
		message.addParam(client_nick);
		message.addParam(channel_name);
		message.setTrailing("You're not channel operator");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);
		return ;
	}

	// - 사용자가 이미 채널에 존재하는가?
	if (!channel_manager_.checkClientIsInChannel(channel_name, target_nick))
	{
		message.setCommand(ERR_USERONCHANNEL);
		message.addParam(client->getHostName());
		message.addParam(client_nick);
		message.addParam(target_nick);
		message.addParam(channel_name);
		message.setTrailing("is already on channel");

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);
		return ;
	}
	
	//2. businness logic

	// - invited 리스트에 추가하고
	channel_manager_.getChannelByName(channel_name)->inviteClient(target_nick);
	
	// - 메시지를 보낸다.
	

	//3. send message

	message




}

void	PacketManager::topic(struct Packet& packet)
{
	
}
void	PacketManager::mode(struct Packet& packet)
{
		
}