#include "PacketMaker.hpp"
#include "macro.h"

void	PacketManager::pass(struct Packet& packet)
{
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	PacketMaker packet_maker(*this);

	std::cout << "[DEBUG] Pass's client_manager_ : " << &client_manager_ << std::endl;
	std::cout << "[DEBUG] client's address : " << client << std::endl;

	if (client->getIsPass())
	{
		packet_maker.ErrAlreadyRegistred(packet);
		return ;
	}

	if (packet.message.getParams().size() != 1)
	{
		packet_maker.ErrNeedMoreParams(packet);
		return ;
	}

	if (packet.message.getParams()[0] != password_)
	{
		packet_maker.ErrPasswdMismatch(packet);
		return ;
	}

	client->setIsPass(true);
}

void	PacketManager::nick(struct Packet& packet)
{
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	PacketMaker packet_maker(*this);

	if (!client->getIsPass())
	{
		packet_maker.ErrNotRegistered(packet);		
		return ;
	}

	if (packet.message.getParams().size() != 1)
	{
		packet_maker.ErrNoNicknameGiven(packet);
		return ;
	}

	std::string old_nick = client->getNickName();
	std::string new_nick = packet.message.getParams()[0];
	if (!client_manager_.isValidNickname(new_nick))
	{
		packet_maker.ErrErroneusNickname(packet);
		return ;
	}

	if (client_manager_.isUsedNickname(new_nick))
	{
		packet_maker.ErrNicknameInUse(packet);
		return ;
	}

	Client *nick_client = client_manager_.getClientByNick(old_nick);
	if (!nick_client)
	{
		client_manager_.addNickClient(new_nick, client);
		client->setNickName(new_nick);

		if (client->getUserName() != "")
		{
			packet_maker.RplWelcome(packet);
			client->setIsAuthenticated(true);
		}
	}
	else
	{
		client_manager_.removeNickClient(old_nick);
		client_manager_.addNickClient(new_nick, client);
		client->setNickName(new_nick);

		Message message = packet_maker.NickSuccess(packet);

		std::vector<Channel *> channels = channel_manager_.getChannelsByClientName(old_nick);
		for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
			(*it)->changeClientInfo(old_nick, new_nick);

			sendPacket(message, (*it), new_nick);
		}
	}
}

void	PacketManager::user(struct Packet& packet)
{
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	PacketMaker packet_maker(*this);

	if (!client->getIsPass())
	{
		packet_maker.ErrNotRegistered(packet);
		return ;
	}

	if (packet.message.getParams().size() != 4)
	{
		packet_maker.ErrNeedMoreParams(packet);
		return ;
	}

	if (client->getUserName() != "")
	{
		packet_maker.ErrAlreadyRegistred(packet);
		return ;
	}

	client->setUserName(packet.message.getParams()[0]);
	client->setHostName(packet.message.getParams()[3]);

	if (client->getNickName() != "")
	{
		packet_maker.RplWelcome(packet);
		client->setIsAuthenticated(true);
	}
}

void	PacketManager::privmsg(struct Packet& packet)
{
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	PacketMaker packet_maker(*this);

	if (!client->getIsAuthenticated())
	{
		packet_maker.ErrNotRegistered(packet);
		return ;
	}

	if (packet.message.getParams().size() != 1)
	{
		packet_maker.ErrNoRecipient(packet);
		return ;
	}

	if (packet.message.getTrailing() == "")
	{
		packet_maker.ErrNoTextToSend(packet);
		return ;
	}

	std::vector<std::string> targets = Message::split(packet.message.getParams()[0], ",");
	for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); it++)
	{
		if (packet.message.getParams()[0][0] == '#')
		{
			Channel *channel = channel_manager_.getChannelByName(packet.message.getParams()[0]);
			if (!channel)
			{
				packet_maker.ErrNoSuchChannel(packet);
				return ;
			}

			if (!channel_manager_.checkClientIsInChannel(packet.message.getParams()[0], client->getNickName()))
			{
				packet_maker.ErrCannotSendToChan(packet);
				return ;
			}

			Message message = packet_maker.PrivmsgToChannel(packet);

			sendPacket(message, channel, client->getNickName());
		}
		else
		{
			Client *target_client = client_manager_.getClientByNick(packet.message.getParams()[0]);
			if (!target_client)
			{
				packet_maker.ErrNoSuchNick(packet);
				return ;
			}

			packet_maker.PrivmsgToUser(packet);
		}
	}
}

void	PacketManager::quit(struct Packet& packet)
{
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	PacketMaker packet_maker(*this);

	if (!client->getIsAuthenticated())
	{
		client->removeClient();
		return ;
	}

	std::set<std::string> channels = client->getChannels();
	for (std::set<std::string>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		Channel *channel = channel_manager_.getChannelByName(*it);
		if (!channel)
			continue ;

		channel_manager_.deleteClientFromChannel(*it, client->getNickName());

		Message message = packet_maker.Quit(packet);

		sendPacket(message, channel);
	}

	client_manager_.removeClient(client->getSocket());
}

void	PacketManager::ping(struct Packet& packet)
{
	PacketMaker packet_maker(*this);

	if (packet.message.getParams().size() != 1)
		return ;

	Message message = packet_maker.Ping(packet);

	struct Packet send_packet = {packet.client_socket, message};
	sendPacket(send_packet);
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
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	PacketMaker packet_maker(*this);

	if (!client->getIsAuthenticated())
	{
		packet_maker.ErrNotRegistered(packet);
		return ;
	}

	if (packet.message.getParams().size() < 1)
	{
		packet_maker.ErrNeedMoreParams(packet);
		return ;
	}
	std::string client_name = client->getNickName();
	std::vector<std::string> channel_names;
	std::vector<std::string> channel_passwords;
	
	if (packet.message.getParams().size() >= 1)
		channel_names = Message::split(packet.message.getParams()[0], ","); 
	if (packet.message.getParams().size() >= 2)
		channel_passwords = Message::split(packet.message.getParams()[1], ",");
	
	std::vector<std::string>::iterator it1 = channel_names.begin();
	std::vector<std::string>::iterator it2 = channel_passwords.begin();
	for (; it1 != channel_names.end(); ++it1, ++it2)
	{
		if ((*it1)[0] != '#')
		{
			packet_maker.ErrBadChanMask(packet);
			continue ;
		}
		if (channel_manager_.getChannelByName(*it1) == NULL)
			channel_manager_.createChannelByName(*it1);
		if (channel_manager_.checkClientIsInChannel(*it1, client_name))
			continue ;
		if (channel_manager_.getChannelByName(*it1)->isOnChannelMode(MODE_PASSWORD))
		{
			if (channel_manager_.checkChannelPassword(*it1, *it2) == false)
			{
				packet_maker.ErrBadChannelKey(packet);
				continue ;
			}
		}
		if (channel_manager_.getChannelByName(*it1)->isOnChannelMode(MODE_INVITE))
		{
			if (channel_manager_.checkClientIsInvited(*it1, client_name) == false)
			{
				packet_maker.ErrInviteOnlyChan(packet);
				continue ;
			}
		}
		if (!(channel_manager_.getChannelByName(*it1)->checkChannelCapacity()))
		{
			packet_maker.ErrChannelIsFull(packet);
			continue ;
		}
		channel_manager_.addClientToChannel(*it1, client_name);
		client_manager_.addChannelToClient(client_name, *it1);
		if (channel_manager_.getChannelByName(*it1)->isOnChannelMode(MODE_TOPIC))
		{
			Packet temp = packet;
			temp.message.setCommand(*it1);
			temp.message.setTrailing(channel_manager_.getChannelByName(*it1)->getTopic());
			packet_maker.RplTopic(temp);
		}
		// broadcast join message
		Packet temp = packet;
		temp.message.setTrailing(*it1);
		packet_maker.RplNamReply(temp);
		packet_maker.BroadcastJoin(temp);
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
	Client *client = client_manager_.getClientBySocket(packet.client_socket);
	PacketMaker packet_maker(*this);
	if (!client->getIsAuthenticated())
	{
		packet_maker.ErrNotRegistered(packet);
		return ;
	}

	if (packet.message.getParams().size() < 1)
	{
		packet_maker.ErrNeedMoreParams(packet);
		return ;
	}
	std::string client_name = client->getNickName();
	std::vector<std::string> channel_names = Message::split(packet.message.getParams()[0], ","); //need split
	std::string part_message = packet.message.getTrailing();

	std::vector<std::string>::iterator it = channel_names.begin();
	for (; it != channel_names.end(); ++it)
	{
		if (channel_manager_.getChannelByName(*it) == NULL)
		{
			packet_maker.ErrNoSuchChannel(packet);
			continue ;
		}
		if (channel_manager_.checkClientIsInChannel(*it, client_name) == false)
		{
			packet_maker.ErrNotOnChannel(packet);
			continue ;
		}

		//broadcast part message
		Packet temp = packet;
		temp.message.setTrailing(*it);
		packet_maker.BroadcastPart(temp);

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
	PacketMaker packet_maker(*this);

	if (!client->getIsAuthenticated())
	{
		packet_maker.ErrNotRegistered(packet);
		return ;
	}
	if (packet.message.getParams().size() < 2)
	{
		packet_maker.ErrNeedMoreParams(packet);
		return ;
	}
	std::string client_name = client->getNickName();
	std::string channel_name = packet.message.getParams()[0];
	std::string target_user_name = packet.message.getParams()[1];
	std::string kick_message = packet.message.getTrailing();
	if (channel_manager_.getChannelByName(channel_name) == NULL)
	{
		packet_maker.ErrNoSuchChannel(packet);
		return ;
	}
	if (client_manager_.getClientByNick(target_user_name) == NULL)
	{
		packet_maker.ErrNoSuchNick(packet);
		return ;
	}
	if (channel_manager_.checkClientIsInChannel(channel_name, client_name) == false)
	{
		packet_maker.ErrUserNotInChannel(packet);
		return ;
	}
	if (channel_manager_.checkClientIsOperator(channel_name, client_name) == false)
	{
		packet_maker.ErrChanOPrivsNeeded(packet);
		return ;
	}
	if (channel_manager_.checkClientIsInChannel(channel_name, target_user_name) == false)
	{
		packet_maker.ErrUserNotInChannel(packet);
		return ;
	}

	//broadcast kick message
	Packet temp = packet;
	temp.message.setPrefix(channel_name);
	temp.message.setCommand(target_user_name);
	packet_maker.BroadcastKick(temp);

	channel_manager_.deleteClientFromChannel(channel_name, target_user_name);
	client_manager_.removeChannelFromClient(target_user_name, channel_name);
}


void	PacketManager::invite(struct Packet& packet)
{
	//0. Argument check

	PacketMaker packet_maker(*this);
	Message message;
	
	std::string client_nick = getNickBySocket(packet.client_socket);
	Client * client = client_manager_.getClientByNick(client_nick);
	
	if (!client->getIsAuthenticated())
	{
		packet_maker.ErrNotRegistered(packet);
		return ;
	}
	
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

	if ((channel_manager_.getChannelByName(channel_name)->isOnChannelMode(MODE_INVITE) && !channel_manager_.checkClientIsOperator(channel_name, client_nick)))
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
	int target_socket = client_manager_.getClientByNick(target_nick)->getSocket();
	std::string inviting_msg = ":" + client_nick + " INVITE " + target_nick + " " + channel_name;
	
	if (send(target_socket, inviting_msg.c_str(), inviting_msg.size(), 0) == -1)
	{
		//error
	}

	//3. send message

	// - RPL_INVITING
	message.setCommand(RPL_INVITING);
	message.addParam(client->getHostName());
	message.addParam(client_nick);
	message.addParam(target_nick);
	message.addParam(channel_name);
	message.setTrailing("Inviting " + target_nick + " to " + channel_name);

	struct Packet pck = {client->getSocket(), message};
	sendPacket(pck);
	return ;
}

void	PacketManager::topic(struct Packet& packet)
{
	//1. error manage

	//### **오류 461: ERR_NEEDMOREPARAMS**
	
	// **오류 메시지 형식**: **`<client> <command> :Not enough parameters`오류 이유**: 클라이언트 명령을 구문 분석할 수 없는 이유는 충분한 매개 변수가 제공되지 않았기 때문입니다.
	// **오류 코드**: **`461`**

	PacketMaker packet_maker(*this);
	Message message;
	
	std::string client_nick = getNickBySocket(packet.client_socket);
	Client * client = client_manager_.getClientByNick(client_nick);

	if (!client->getIsAuthenticated())
	{
		packet_maker.ErrNotRegistered(packet);
		return ;
	}

	if ( packet.message.getParams().size() != 1 || packet.message.getTrailing().size() != 0)
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

	std::string channel_name = packet.message.getParams()[0];
	Channel *channel = channel_manager_.getChannelByName(channel_name);

	// ### **오류 403: ERR_NOSUCHCHANNEL**

	// **오류 메시지 형식**: **`<client> <channel> :No such channel`오류 이유**: 제공된 채널 이름에 대한 채널을 찾을 수 없다는 것을 나타냅니다.
	// **오류 코드**: **`403`**

	if (!channel_manager_.getChannelByName(channel_name))
	{
		message.setCommand(ERR_NOSUCHCHANNEL);
		message.addParam(client->getHostName());
		message.addParam(client_nick);
		message.addParam(channel_name);
		message.setTrailing(ERR_NOSUCHCHANNEL_MSG);

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);
		return ;
	}

	// ### **오류 442: ERR_NOTONCHANNEL**
	// **오류 메시지 형식**: **`<client> <channel> :You're not on that channel`오류 이유**: 클라이언트가 해당 채널의 일부가 아닌 채널에 영향을 주는 명령을 수행하려고 할 때 반환됩니다.
	// **오류 코드**: **`442`**
	
	if (!channel_manager_.checkClientIsInChannel(channel_name, client_nick))
	{
		message.setCommand(ERR_NOTONCHANNEL);
		message.addParam(client->getHostName());
		message.addParam(client_nick);
		message.addParam(channel_name);
		message.setTrailing(ERR_NOTONCHANNEL_MSG);

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);
		return ;
	}


	

	// ### **오류 482: ERR_CHANOPRIVSNEEDED**

	// **오류 메시지 형식**: **`<client> <channel> :You're not channel operator`오류 이유**: 클라이언트가 적절한 채널 권한이 없어 명령이 실패했다는 것을 나타냅니다.
	// **오류 코드**: **`482`**
	if (channel->isOnChannelMode(MODE_TOPIC) && !channel_manager_.checkClientIsOperator(channel_name, client_nick))
	{
		message.setCommand(ERR_CHANOPRIVSNEEDED);
		message.addParam(client->getHostName());
		message.addParam(client_nick);
		message.addParam(channel_name);
		message.setTrailing(ERR_CHANOPRIVSNEEDED_MSG);

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);
		return ;
	}


	//2. business logic

	std::string topic = packet.message.getTrailing();
	if (topic.empty() != 0)
	{
		channel->setTopic(topic);
		channel->setTopicSetter(client_nick);
		channel->setTopicSetTime();

		message.setCommand(RPL_TOPIC);
		message.addParam(client->getHostName());
		message.addParam(client_nick);
		message.addParam(channel_name);
		message.setTrailing(topic);

		struct Packet packet = {client->getSocket(), message};
		sendPacket(packet);

		Message message2;

		message2.setCommand(RPL_TOPICWHOTIME);
		message2.addParam(client->getHostName());
		message2.addParam(client_nick);
		message2.addParam(channel_name);
		message2.addParam(channel->getTopicSetter());
		message2.addParam(channel->getTopicSetTime());

		struct Packet packet2 = {client->getSocket(), message2};
		sendPacket(packet2);	
	}
	else
	{
		topic = channel->getTopic();
		if (topic.empty() != 0)
		{
			message.setCommand(RPL_TOPIC);
			message.addParam(client->getHostName());
			message.addParam(client_nick);
			message.addParam(channel_name);
			message.setTrailing(topic);

			struct Packet packet = {client->getSocket(), message};
			sendPacket(packet);

			Message message2;

			message2.setCommand(RPL_TOPICWHOTIME);
			message2.addParam(client->getHostName());
			message2.addParam(client_nick);
			message2.addParam(channel_name);
			message2.addParam(channel->getTopicSetter());
			message2.addParam(channel->getTopicSetTime());

			struct Packet packet2 = {client->getSocket(), message2};
			sendPacket(packet2);
		} 
		else
		{
			message.setCommand(RPL_NOTOPIC);
			message.addParam(client->getHostName());
			message.addParam(client_nick);
			message.addParam(channel_name);
			message.setTrailing(RPL_NOTOPIC_MSG);

			struct Packet packet = {client->getSocket(), message};
			sendPacket(packet);
		}
	}

	// ### **응답 331: RPL_NOTOPIC**
	// **응답 메시지 형식**: **`<client> <channel> :No topic is set`응답 설명**: 클라이언트가 채널에 가입할 때 해당 채널에 설정된 주제가 없음을 알립니다.
	
	// ### **응답 332: RPL_TOPIC**
	// **응답 메시지 형식**: **`<client> <channel> :<topic>`응답 설명**: 클라이언트가 채널에 가입할 때 해당 채널의 현재 주제를 알려줍니다.

	// ### **응답 333: RPL_TOPICWHOTIME**

	// **응답 메시지 형식**: **`<client> <channel> <nick> <setat>`응답 설명**: 주제를 설정한 사람(<nick>)과 주제를 설정한 시간(<setat>은 유닉스 타임스탬프)을 알려줍니다. 이는 RPL_TOPIC(332) 이후에 전송됩니다.

	//3. send message success

}

// void	PacketManager::mode(struct Packet& packet)
// {
// 	std::string client_nick = getNickBySocket(packet.client_socket);
// 	Client * client = client_manager_.getClientByNick(client_nick);

// 	std::string channel_name = packet.message.getParams()[0];
// 	Channel *channel = channel_manager_.getChannelByName(channel_name);

// 	std::string mode_str = packet.message.getParams()[1];
// 	std::string mode_par = packet.message.getParams()[2];
	


// 	//business logic


// 	//mode handling
// 	int iter = 0;
// 	while (iter < mode_str.size())
// 	{
// 		if (mode_str[iter] == '+' )

// 		iter ++;
// 	}

// }
