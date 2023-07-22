#include "Channel.hpp"
#include <iostream>

Channel::Channel(const std::string& name)
{
	if (name[0] != '#')
		channel_name_ = "#" + name;
	else
		channel_name_ = name;
	limit_ = -1;
	mode_ = "nt";
	setChannelCreatedTime();
}

Channel::~Channel()
{
	operators_.clear();
	clients_.clear();
	invited_clients_.clear();
}

void	Channel::setChannelCreatedTime()
{
	struct timeval	tv;
	gettimeofday(&tv, NULL);
	channel_created_time_ = tv.tv_sec;
}

//getChannelCreatedTime()

unsigned long	Channel::getChannelCreatedTime(void)
{
	return channel_created_time_;
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
	struct timeval	tv;
	gettimeofday(&tv, NULL);
	topic_settime_ = tv.tv_sec;
}

unsigned long	Channel::getTopicSetTime(void)
{
	return topic_settime_;
}

void	Channel::setLimit(int limit)
{
	limit_ = limit;
}

int		Channel::getLimit(void)
{
	return limit_;
}

std::string	Channel::getChannelName(void)
{
	return channel_name_;
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
    if (clients_.find(client_name) != clients_.end())
        return true;
    else
        return false;
}

void	Channel::setOperator(const std::string& client_name) // 제대로 작동 X
{
	std::cout << "[INFO] setOperator is checking for " << client_name << std::endl;
	for (std::set<std::string>::iterator it = clients_.begin(); it != clients_.end(); ++it)
		std::cout << "[INFO] Client : " << *it << std::endl;
	if (checkClientIsInChannel(client_name))
	{
		operators_.insert(client_name);
		std::cout << "[INFO] " << client_name << " is now operator of " << channel_name_ << std::endl;
	}
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

std::string	Channel::getClientsString(void)
{
	std::string result;
	std::set<std::string>::iterator it = clients_.begin();
	for (; it != clients_.end(); ++it)
	{
		if (operators_.find(*it) != operators_.end())
			result += "@";
		result += *it;
		result += " ";
	}
	return result;
}


bool	Channel::isOnChannelMode(char mode)
{
	if (mode_.find(mode) != std::string::npos)
		return true;
	else
		return false;
}

void	Channel::setChannelMode(char mode)
{
	if (!isOnChannelMode(mode))
		mode_ += mode;
}

std::string	Channel::getChannelModeString(void)
{
	return mode_;
}


void	Channel::unsetChannelMode(char mode)
{
	if (isOnChannelMode(mode))
		mode_.erase(mode_.find(mode), 1);
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