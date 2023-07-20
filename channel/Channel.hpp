#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>

#include "../client/Client.hpp"
#include "ChannelMode.hpp"

class Channel {
	public:
		std::string channel_name_;
		unsigned long channel_created_time_;

		std::string topic_;
		std::string topic_setter_;
		//write topic settime unix timestamp
		unsigned long topic_settime_;

		std::string password_;
		std::set<std::string> operators_;
		std::set<std::string> clients_; // 필요한  경우, 바로 map<std::string, *client> 사용할 것!
		std::set<std::string> invited_clients_;
		int limit_;
		int mode_;

		Channel(const std::string& name);
		~Channel();

		void	setChannelCreatedTime();

		void	setTopic(const std::string& name);
		const std::string&	getTopic(void);

		void	setTopicSetter(const std::string& name);
		std::string&	getTopicSetter(void);

		void	setTopicSetTime(unsigned long time);
		unsigned long	getTopicSetTime(void);


		void	setPassword(const std::string& pw);
		bool	checkPassword(const std::string& pw);
		void	clearPassword(void);

		void	addClient(const std::string& client_name);
		void	inviteClient(const std::string& client_name);
		void	deleteClient(const std::string& client_name);
		
		bool	checkClientIsOperator(const std::string& client_name);
		bool	checkClientIsInChannel(const std::string& client_name);
		bool	checkClientIsInvited(const std::string& client_name);
		
		void	setOperator(const std::string& client_name);
		void	unsetOperator(const std::string& client_name);

		bool	checkChannelCapacity(void);

		const int getChannelMode(void);
		void	setChannelMode(int channel_mode);

		void	changeClientInfo(const std::string& client_name, const std::string& new_name);
};

#endif