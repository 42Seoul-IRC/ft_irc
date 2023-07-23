#include "PacketManager.hpp"
#include "macro.h"

void	PacketManager::cap(struct Packet& packet)
{
	if (packet.message.getParams().size() == 0)
	{
		packet_maker_->ErrNeedMoreParams(packet);
		return ;
	}
}

void	PacketManager::pass(struct Packet& packet)
{
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	if (client->getIsPass())
	{
		packet_maker_->ErrAlreadyRegistred(packet);
		return ;
	}

	if (packet.message.getParams().size() != 1)
	{
		packet_maker_->ErrNeedMoreParams(packet);
		return ;
	}

	if (packet.message.getParams()[0] != password_)
	{
		packet_maker_->ErrPasswdMismatch(packet);
		return ;
	}

	client->setIsPass(true);
}

void	PacketManager::nick(struct Packet& packet)
{
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	if (!client->getIsPass())
	{
		packet_maker_->ErrNotRegistered(packet);		
		return ;
	}

	if (packet.message.getParams().size() != 1)
	{
		packet_maker_->ErrNoNicknameGiven(packet);
		return ;
	}

	std::string old_nick = client->getNickName();
	std::string new_nick = packet.message.getParams()[0];
	if (!client_manager_.isValidNickname(new_nick))
	{
		packet_maker_->ErrErroneusNickname(packet);
		return ;
	}

	if (client_manager_.isUsedNickname(new_nick))
	{
		packet_maker_->ErrNicknameInUse(packet);
		return ;
	}

	Client *nick_client = client_manager_.getClientByNick(old_nick);
	if (!nick_client)
	{
		client_manager_.addNickClient(new_nick, client);
		client->setNickName(new_nick);

		if (client->getUserName() != "")
		{
			packet_maker_->RplWelcome(packet);
			client->setIsAuthenticated(true);
		}
	}
	else
	{
		Message message = packet_maker_->NickSuccess(packet);
		
		client_manager_.removeNickClient(old_nick);
		client_manager_.addNickClient(new_nick, client);
		client->setNickName(new_nick);

		std::vector<Channel *> channels = channel_manager_.getChannelsByClientName(old_nick);
		for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
			(*it)->changeClientInfo(old_nick, new_nick);

			packet_maker_->sendPacket(message, (*it)->getChannelName(), new_nick);
		}
	}
}

void	PacketManager::user(struct Packet& packet)
{
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	if (!client->getIsPass())
	{
		packet_maker_->ErrNotRegistered(packet);
		return ;
	}

	if (packet.message.getParams().size() != 3)
	{
		packet_maker_->ErrNeedMoreParams(packet);
		return ;
	}

	if (client->getUserName() != "")
	{
		packet_maker_->ErrAlreadyRegistred(packet);
		return ;
	}

	client->setUserName(packet.message.getParams()[0]);
	client->setHostName(packet.message.getParams()[2]);

	if (client->getNickName() != "")
	{
		packet_maker_->RplWelcome(packet);
		client->setIsAuthenticated(true);
	}
}

void	PacketManager::privmsg(struct Packet& packet)
{
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

	if (!client->getIsAuthenticated())
	{
		packet_maker_->ErrNotRegistered(packet);
		return ;
	}

	if (packet.message.getParams().size() != 1)
	{
		packet_maker_->ErrNoRecipient(packet);
		return ;
	}

	if (packet.message.getTrailing() == "")
	{
		packet_maker_->ErrNoTextToSend(packet);
		return ;
	}

	std::vector<std::string> targets = Message::split(packet.message.getParams()[0], ",");
	for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); it++)
	{
		if ((*it)[0] == '#')
		{
			Channel *channel = channel_manager_.getChannelByName(*it);
			if (!channel)
			{
				packet_maker_->ErrNoSuchChannel(packet, *it);
				return ;
			}

			if (!channel_manager_.checkClientIsInChannel(*it, client->getNickName()))
			{
				packet_maker_->ErrCannotSendToChan(packet, *it);
				return ;
			}

			packet_maker_->PrivmsgToChannel(packet, *it);
		}
		else
		{
			Client *target_client = client_manager_.getClientByNick(*it);
			if (!target_client)
			{
				packet_maker_->ErrNoSuchNick(packet, *it);
				return ;
			}

			packet_maker_->PrivmsgToUser(packet, *it);
		}
	}
}

void	PacketManager::quit(struct Packet& packet)
{
	Client *client = client_manager_.getClientBySocket(packet.client_socket);

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

		Message message = packet_maker_->Quit(packet);

		packet_maker_->sendPacket(message, channel->getChannelName());
	}

	client_manager_.removeClient(client->getSocket());
}

void	PacketManager::ping(struct Packet& packet)
{
	if (packet.message.getParams().size() != 1)
		return ;

	Message message = packet_maker_->Ping(packet);

	struct Packet send_packet = {packet.client_socket, message};
	packet_maker_->sendPacket(send_packet);
}


void	PacketManager::join(struct Packet& packet)
{
/*
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

	if (!client->getIsAuthenticated())
	{
		packet_maker_->ErrNotRegistered(packet);
		return ;
	}

	if (packet.message.getParams().size() < 1)
	{
		packet_maker_->ErrNeedMoreParams(packet);
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
			packet_maker_->ErrBadChanMask(packet);
			continue ;
		}
		if (channel_manager_.getChannelByName(*it1) == NULL)
			channel_manager_.createChannelByName(*it1);
		if (channel_manager_.checkClientIsInChannel(*it1, client_name))
			continue ;
		if (channel_manager_.getChannelByName(*it1)->isOnChannelMode(MODE_PASSWORD))
		{
			if (it2 == channel_passwords.end() || channel_manager_.checkChannelPassword(*it1, *it2) == false)
			{
				packet.message.setPrefix(*it1);
				packet_maker_->ErrBadChannelKey(packet);
				continue ;
			}
		}
		if (channel_manager_.getChannelByName(*it1)->isOnChannelMode(MODE_INVITE))
		{
			if (channel_manager_.checkClientIsInvited(*it1, client_name) == false)
			{
				packet.message.setPrefix(*it1);
				packet_maker_->ErrInviteOnlyChan(packet);
				continue ;
			}
		}
		if (!(channel_manager_.getChannelByName(*it1)->checkChannelCapacity()))
		{
			packet.message.setPrefix(*it1);
			packet_maker_->ErrChannelIsFull(packet);
			continue ;
		}
		channel_manager_.addClientToChannel(*it1, client_name);
		client_manager_.addChannelToClient(client_name, *it1);
		if (channel_manager_.getChannelByName(*it1)->isOnChannelMode(MODE_TOPIC))
		{
			Packet temp = packet;
			temp.message.setCommand(*it1);
			temp.message.setTrailing(channel_manager_.getChannelByName(*it1)->getTopic());
			packet_maker_->RplTopic(temp);
		}
		// broadcast join message
		Packet temp = packet;
		temp.message.setTrailing(*it1);
		packet_maker_->RplNamReply(temp);
		packet_maker_->RplEndOfNames(temp);
		packet_maker_->BroadcastJoin(temp);
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

	if (!client->getIsAuthenticated())
	{
		packet_maker_->ErrNotRegistered(packet);
		return ;
	}

	if (packet.message.getParams().size() < 1)
	{
		packet_maker_->ErrNeedMoreParams(packet);
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
			packet_maker_->ErrNoSuchChannel(packet, *it);
			continue ;
		}
		if (channel_manager_.checkClientIsInChannel(*it, client_name) == false)
		{
			packet_maker_->ErrNotOnChannel(packet);
			continue ;
		}

		//broadcast part message
		Packet temp = packet;
		temp.message.setCommand(*it);
		packet_maker_->BroadcastPart(temp);

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

	if (!client->getIsAuthenticated())
	{
		packet_maker_->ErrNotRegistered(packet);
		return ;
	}
	if (packet.message.getParams().size() < 2)
	{
		packet_maker_->ErrNeedMoreParams(packet);
		return ;
	}
	std::string client_name = client->getNickName();
	std::string channel_name = packet.message.getParams()[0];
	std::string target_user_name = packet.message.getParams()[1];
	std::string kick_message = packet.message.getTrailing();
	if (channel_manager_.getChannelByName(channel_name) == NULL)
	{
		packet_maker_->ErrNoSuchChannel(packet);
		return ;
	}
	if (client_manager_.getClientByNick(target_user_name) == NULL)
	{
		packet_maker_->ErrNoSuchNick(packet);
		return ;
	}
	if (channel_manager_.checkClientIsInChannel(channel_name, client_name) == false)
	{
		packet_maker_->ErrUserNotInChannel(packet);
		return ;
	}
	if (channel_manager_.checkClientIsOperator(channel_name, client_name) == false)
	{
		packet.message.setPrefix(channel_name);
		packet_maker_->ErrChanOPrivsNeeded(packet);
		return ;
	}
	if (channel_manager_.checkClientIsInChannel(channel_name, target_user_name) == false)
	{
		packet_maker_->ErrUserNotInChannel(packet);
		return ;
	}

	//broadcast kick message
	Packet temp = packet;
	temp.message.setPrefix(channel_name);
	temp.message.setCommand(target_user_name);
	packet_maker_->BroadcastKick(temp);

	channel_manager_.deleteClientFromChannel(channel_name, target_user_name);
	client_manager_.removeChannelFromClient(target_user_name, channel_name);
}


void	PacketManager::invite(struct Packet& packet)
{
	//0. Argument check
	Message message;
	
	std::string client_nick = getNickBySocket(packet.client_socket);
	Client * client = client_manager_.getClientByNick(client_nick);

	if (!client->getIsAuthenticated())
	{
		packet_maker_->ErrNotRegistered(packet);
		return ;
	}	

	if (packet.message.getParams().size() != 2 && packet.message.getParams().size() < 3)
	{
		// ERR_NEEDMOREPARAMS
		packet_maker_->ErrNeedMoreParams(packet);
		return ;
	}
	
	std::string target_nick = packet.message.getParams()[0];
	std::string channel_name = packet.message.getParams()[1];

	
	//1. validity check
	
	//target is in server?
	if (!client_manager_.getClientByNick(target_nick))
	{
		packet_maker_->ErrNoSuchNick(packet);
		return ;
	}

	
	//target is autu?
	if (!client_manager_.getClientByNick(client_nick)->getIsAuthenticated())
	{
		packet_maker_->ErrNotRegistered(packet);
		return ;
	}


	//chennel exits?
	if (!channel_manager_.getChannelByName(channel_name))
	{
		packet_maker_->ErrNoSuchChannel(packet);
		return ;
	}
	
	//- 명령어를 보낸 사용자가 채널 안에 존재하는가?
	if (!channel_manager_.checkClientIsInChannel(channel_name, client_nick))
	{
		packet_maker_->ErrNotOnChannel(packet);
		return ;
	}

	// - invite 모두가 켜져있는 경우에는, operator가 아닌 사람은 초대할 수 없다. 

	if ((channel_manager_.getChannelByName(channel_name)->isOnChannelMode(MODE_INVITE) && !channel_manager_.checkClientIsOperator(channel_name, client_nick)))
	{
		packet_maker_->ErrChanOPrivsNeeded(packet);
		return ;
	}

	// - 사용자가 이미 채널에 존재하는가?
	if (channel_manager_.checkClientIsInChannel(channel_name, target_nick))
	{
		packet_maker_->ErrUserOnChannel(packet);
		return ;
	}
	
	//2. businness logic

	// - invited 리스트에 추가하고
	channel_manager_.getChannelByName(channel_name)->inviteClient(target_nick);

	
	// - 메시지를 보낸다.


	//3. send message

	// - RPL_INVITING

	packet_maker_->RplInviting(packet);
	packet_maker_->msgToUser(packet, "INVITE",target_nick);
	
	return ;
}

void	PacketManager::topic(struct Packet& packet)
{
	//1. error manage

	//### **오류 461: ERR_NEEDMOREPARAMS**
	
	// **오류 메시지 형식**: **`<client> <command> :Not enough parameters`오류 이유**: 클라이언트 명령을 구문 분석할 수 없는 이유는 충분한 매개 변수가 제공되지 않았기 때문입니다.
	// **오류 코드**: **`461`**

	Message message;
	
	std::string client_nick = getNickBySocket(packet.client_socket);
	Client * client = client_manager_.getClientByNick(client_nick);

	if (!client->getIsAuthenticated())
	{
		packet_maker_->ErrNotRegistered(packet);
		return ;
	}

	if (packet.message.getParams().size() == 0)
	{
		// ERR_NEEDMOREPARAMS
		packet_maker_->ErrNeedMoreParams(packet);
		return ;
	}	

	std::string channel_name = packet.message.getParams()[0];
	Channel *channel = channel_manager_.getChannelByName(channel_name);

	// ### **오류 403: ERR_NOSUCHCHANNEL**

	// **오류 메시지 형식**: **`<client> <channel> :No such channel`오류 이유**: 제공된 채널 이름에 대한 채널을 찾을 수 없다는 것을 나타냅니다.
	// **오류 코드**: **`403`**

	if (!channel_manager_.getChannelByName(channel_name))
	{
		packet_maker_->ErrNoSuchChannel(packet);
		return ;
	}

	// ### **오류 442: ERR_NOTONCHANNEL**
	// **오류 메시지 형식**: **`<client> <channel> :You're not on that channel`오류 이유**: 클라이언트가 해당 채널의 일부가 아닌 채널에 영향을 주는 명령을 수행하려고 할 때 반환됩니다.
	// **오류 코드**: **`442`**
	
	if (!channel_manager_.checkClientIsInChannel(channel_name, client_nick))
	{
		packet_maker_->ErrNotOnChannel(packet);
		return ;
	}


	




	//2. business logic

	std::string topic = packet.message.getTrailing();
	if (!topic.empty() || packet.message.getHasTrailing())
	{
		// ### **오류 482: ERR_CHANOPRIVSNEEDED**

		// **오류 메시지 형식**: **`<client> <channel> :You're not channel operator`오류 이유**: 클라이언트가 적절한 채널 권한이 없어 명령이 실패했다는 것을 나타냅니다.
		// **오류 코드**: **`482`**
		if (channel->isOnChannelMode(MODE_TOPIC) && !channel_manager_.checkClientIsOperator(channel_name, client_nick))
		{
			packet.message.setPrefix(channel_name);
			packet_maker_->ErrChanOPrivsNeeded(packet);
			return ;
		}

		channel->setTopic(topic);
		channel->setTopicSetter(client_nick);
		channel->setTopicSetTime();
		
		// packet_maker_->RplTopic(packet);
		// packet_maker_->RplTopicWhoTime(packet);		
		
		packet_maker_->BroadcastTopic(packet);
		return ;
	}
	else
	{
		std::string pre_topic = channel->getTopic();
		if (!pre_topic.empty())
		{
			packet_maker_->RplTopic(packet, pre_topic);
			packet_maker_->RplTopicWhoTime(packet);
		} 
		else
		{
			packet_maker_->RplNoTopic(packet);
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