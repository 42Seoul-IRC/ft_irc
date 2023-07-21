#include "ChannelManager.hpp"
#include <iostream>

ChannelManager::~ChannelManager()
{
	std::map<std::string, Channel*>::iterator it;

	for (it = channels_.begin(); it != channels_.end(); it++)
	{
		delete it->second;
	}
	channels_.clear();
}

void	ChannelManager::createChannelByName(const std::string& channel_name)
{
	if (channels_.find(channel_name) != channels_.end())
	{
		std::cout << "[ERROR] ChannelManager::createChannelByName : " << channel_name << " is already exist." << std::endl;
		return ;
	}
	Channel *temp = new Channel(channel_name);
	channels_.insert(std::make_pair(channel_name, temp));
	std::cout << "[INFO] Create channel : " << channel_name  << ", " << temp << std::endl;
}

void	ChannelManager::addClientToChannel(const std::string& channel_name, const std::string& client_name)
{
	Channel *temp = getChannelByName(channel_name);
	if (temp == NULL)
	{
		std::cout << "[ERROR] ChannelManager::addClientToChannel : " << channel_name << " is not exist." << std::endl;
		return ;
	}
	temp->addClient(client_name);
	if (temp->clients_.size() == 1)
	{
		temp->setOperator(client_name);
		std::cout << "[INFO] ChannelManager::addClientToChannel : " << channel_name << " has set " << client_name << " to operator." << std::endl;
	}
}

Channel	*ChannelManager::getChannelByName(const std::string& channel_name)
{
	if (channels_.find(channel_name) == channels_.end())
		return (NULL);
	return (channels_.find(channel_name)->second);
}

void	ChannelManager::deleteChannelByName(const std::string& channel_name)
{
	if (channels_.find(channel_name) == channels_.end())
	{
		std::cout << "[ERROR] ChannelManager::deleteChannelByName : " << channel_name << " is not exist." << std::endl;
		return ;
	}
	delete channels_.find(channel_name)->second;
	channels_.erase(channel_name);
}

void	ChannelManager::deleteClientFromChannel(const std::string& channel_name, const std::string& client_name)
{
	// delete client_name from channel
	// if channel->clients_.length() == 0 : delete channel
	if (channels_.find(channel_name) == channels_.end())
		return ;
	channels_.find(channel_name)->second->deleteClient(client_name);
	if (channels_.find(channel_name)->second->clients_.size() == 0)
		deleteChannelByName(channel_name);
}

int		ChannelManager::getChannelOnClientCount(const std::string& channel_name)
{
	if (channels_.find(channel_name) == channels_.end())
		return (-1);
	return (channels_.find(channel_name)->second->clients_.size());
}

// int		ChannelManager::getChannelMode(const std::string& channel_name)
// {
// 	if (channels_.find(channel_name) == channels_.end())
// 		return (-1);
// 	return (channels_.find(channel_name)->second->getChannelMode());
// }

std::vector<Channel *> ChannelManager::getChannelsByClientName(const std::string& clientName)
{
	std::vector<Channel *> include;
	for (std::map<std::string, Channel *>::iterator it = channels_.begin(); it != channels_.end(); it++)
	{
		if (it->second->checkClientIsInChannel(clientName))
			include.push_back(it->second);
	}
	return include;
}

bool 	ChannelManager::checkClientIsOperator(const std::string& channel_name, const std::string& clientName)
{
	if (channels_.find(channel_name) == channels_.end())
		return (false);
	return (channels_.find(channel_name)->second->checkClientIsOperator(clientName));	
}

bool 	ChannelManager::checkClientIsInvited(const std::string& channel_name, const std::string& clientName)
{
	if (channels_.find(channel_name) == channels_.end())
		return (false);
	return (channels_.find(channel_name)->second->checkClientIsInvited(clientName));
}

bool 	ChannelManager::checkClientIsInChannel(const std::string& channel_name, const std::string& clientName)
{
	if (channels_.find(channel_name) == channels_.end())
		return (false);
	return (channels_.find(channel_name)->second->checkClientIsInChannel(clientName));
}

// true if password is right
bool	ChannelManager::checkChannelPassword(const std::string& channel_name, const std::string& password)
{
	if (channels_.find(channel_name) == channels_.end())
		return (false);
	return (channels_.find(channel_name)->second->checkPassword(password));
}
