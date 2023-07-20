#include "Channel.hpp"

Channel::Channel(const std::string& name)
{
	if (name[0] != '#')
		channel_name_ = "#" + name;
	else
		channel_name_ = name;
	limit_ = -1;
	mode_ = 0;
}

Channel::~Channel()
{
	operators_.clear();
	clients_.clear();
	invited_clients_.clear();
}

void	Channel::setChannelCreatedTime()
{
	channel_created_time_ = time(NULL);
}

void	Channel::setTopic(const std::string& name)
{
	topic_ = name;
}

const std::string&	Channel::getTopic(void)
{
	return topic_;
}

void	Channel::setTopicSetter(const std::string& name)
{
	topic_setter_ = name;
}

std::string&	Channel::getTopicSetter(void)
{
	return topic_setter_;
}

void	Channel::setTopicSetTime()
{
	topic_settime_ = time(NULL);
}

unsigned long	Channel::getTopicSetTime(void)
{
	return topic_settime_;
}


void	Channel::setPassword(const std::string& pw)
{
	password_ = pw;
}

bool	Channel::checkPassword(const std::string& pw)
{
	if (password_ == pw)
		return true;
	else
		return false;
}

void	Channel::clearPassword(void)
{
	password_.clear();
}

void	Channel::addClient(const std::string& client_name)
{
	clients_.insert(client_name);
	if (checkClientIsInvited(client_name))
		invited_clients_.erase(client_name);
}

void	Channel::inviteClient(const std::string& client_name)
{
	invited_clients_.insert(client_name);
}

void	Channel::deleteClient(const std::string& client_name)
{
	if (checkClientIsOperator(client_name))
		unsetOperator(client_name);
	if (checkClientIsInvited(client_name))
		invited_clients_.erase(client_name);
	clients_.erase(client_name);
}

bool Channel::checkClientIsOperator(const std::string& client_name)
{
    if (operators_.find(client_name) != operators_.end())
        return true;
    else
        return false;

}

bool Channel::checkClientIsInvited(const std::string& client_name)
{
    if (invited_clients_.find(client_name) != invited_clients_.end())
        return true;
    else
        return false;
}

bool Channel::checkClientIsInChannel(const std::string& client_name)
{
    if(clients_.find(client_name) != clients_.end())
        return true;
    else
        return false;
}

void	Channel::setOperator(const std::string& client_name)
{
	if (checkClientIsInChannel(client_name))
		operators_.insert(client_name);
}

void	Channel::unsetOperator(const std::string& client_name)
{
	if (checkClientIsOperator(client_name))
		operators_.erase(client_name);
}

bool	Channel::checkChannelCapacity(void)
{
	if (limit_ == -1)
		return true;
	else if (limit_ > (int)clients_.size())
		return true;
	else
		return false;
}

const int Channel::getChannelMode(void)
{
	return mode_;
}

void	Channel::setChannelMode(int channel_mode)
{
	mode_ ^= channel_mode;
}

void	Channel::changeClientInfo(const std::string& client_name, const std::string& new_nick)
{
	clients_.erase(client_name);
	clients_.insert(new_nick);
	if (checkClientIsOperator(client_name))
	{
		unsetOperator(client_name);
		setOperator(new_nick);
	}
	if (checkClientIsInvited(client_name))
	{
		invited_clients_.erase(client_name);
		invited_clients_.insert(new_nick);
	}
}