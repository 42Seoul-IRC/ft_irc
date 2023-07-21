#ifndef PACKETMAKER_HPP
#define PACKETMAKER_HPP

#include "PacketManager.hpp"
#include "macro.h"

class PacketMaker : public PacketManager
{
	public:
		// Common Error
		void ErrNotRegistered(struct Packet& packet);
		void ErrNeedMoreParams(struct Packet& packet);
		void ErrNoSuchChannel(struct Packet& packet);
		void ErrNoSuchNick(struct Packet& packet);
		void ErrNotOnChannel(struct Packet& packet);
		void ErrChanOPrivsNeeded(struct Packet& packet);
		
		// RPL
		void RplWelcome(struct Packet& packet);
		void RplNoTopic(struct Packet& packet);
		void RplTopic(struct Packet& packet);
		void RplTopicWhoTime(struct Packet& packet);
		void RplNamReply(struct Packet& packet);
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
		void ErrCannotSendToChan(struct Packet& packet);
		// PRIVMSG Success
		Message PrivmsgToChannel(struct Packet& packet);
		void PrivmsgToUser(struct Packet& packet);

		// QUIT Success
		Message Quit(struct Packet& packet);

		// PING Success
		Message Ping(struct Packet& packet);

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
};

#endif