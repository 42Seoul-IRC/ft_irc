#ifndef PACKETMAKER_HPP
# define PACKETMAKER_HPP

#include "PacketManager.hpp"
#include "macro.h"

class PacketMaker
{
	public:
		PacketMaker(ClientManager& client_manager_, ChannelManager& channel_manager_);
		~PacketMaker();

		ClientManager& client_manager_;
		ChannelManager& channel_manager_;
		struct Packet packet;

		void log(struct Packet& packet);
		void sendPacket(struct Packet& packet);
		void sendPacket(Message message, Channel *channel);
		void sendPacket(Message message, Channel *channel, std::string exclude_nick);

		// Connection
		void CapLs(struct Packet& packet);

		// Common Error
		void ErrNotRegistered(struct Packet& packet);
		void ErrNeedMoreParams(struct Packet& packet);
		void ErrNoSuchChannel(struct Packet& packet, std::string channel_name);
		void ErrNoSuchChannel(struct Packet& packet);
		void ErrNoSuchNick(struct Packet& packet, std::string nick_name);
		void ErrNoSuchNick(struct Packet& packet);
		void ErrNotOnChannel(struct Packet& packet);
		void ErrChanOPrivsNeeded(struct Packet& packet);
		
		// RPL
		void RplWelcome(struct Packet& packet);
		void RplNoTopic(struct Packet& packet);
		void RplTopic(struct Packet& packet);
		void RplTopic(struct Packet& packet, std::string pre_topic);
		void RplTopicWhoTime(struct Packet& packet);
		void RplNamReply(struct Packet& packet);
		void RplEndOfNames(struct Packet& packet);
		void RplInviting(struct Packet& packet);
		void RplChannelModeIs(struct Packet& packet, Channel *channel);
		void RplCreationTime(struct Packet& packet, Channel *channel);

		// PASS Error
		void ErrAlreadyRegistered(struct Packet& packet);
		void ErrPasswdMismatch(struct Packet& packet);

		// NICK Error
		void ErrNoNicknameGiven(struct Packet& packet);
		void ErrErroneusNickname(struct Packet& packet);
		void ErrNicknameInUse(struct Packet& packet);
		// NICK Success
		Message NickSuccess(struct Packet& packet);
		
		// USER Error
		void ErrAlreadyRegistred(struct Packet& packet);

		// PRIVMSG Error
		void ErrNoRecipient(struct Packet& packet);
		void ErrNoTextToSend(struct Packet& packet);
		void ErrCannotSendToChan(struct Packet& packet, std::string channel_name);
		// PRIVMSG Success
		void PrivmsgToChannel(struct Packet& packet, std::string target_channel);
		void PrivmsgToUser(struct Packet& packet, std::string target_nick);

		// QUIT Success
		Message Quit(struct Packet& packet);

		// PING Success
		Message Ping(struct Packet& packet);
		

		// TOPIC 		
		void BroadcastTopic(struct Packet& packet);

		// JOIN Error
		void ErrBadChannelKey(struct Packet& packet);
		void ErrInviteOnlyChan(struct Packet& packet);
		void ErrChannelIsFull(struct Packet& packet);
		void ErrBadChanMask(struct Packet& packet);
		// JOIN Success
		void BroadcastJoin(struct Packet& packet);

		// PART Success
		void BroadcastPart(struct Packet& packet);
		
		// KICK Error
		void ErrUserNotInChannel(struct Packet& packet);
		// KICK Success
		void BroadcastKick(struct Packet& packet);

		// INVITE Error
		void ErrUserOnChannel(struct Packet& packet);
		void msgToUser(struct Packet& packet, const std::string command, std::string target_nick);
		void BroadcastMode(Channel *channel, std::string changed_mode_buffer, std::string param_buffer);

		// MODE Error
		void ErrNeedMoreParams(struct Packet& packet, char mode);
		void ErrNeedMoreParamsOp(struct Packet& packet);
		void ErrNeedMoreParamsKey(struct Packet& packet);
		void ErrNeedMoreParamsLimit(struct Packet& packet);

};

#endif