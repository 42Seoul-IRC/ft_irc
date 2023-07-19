#include "Client.hpp"

Client::Client(int socket)
{
	client_socket_ = socket;
}

const int Client::getSocket(void) const
{
    return client_socket_;
}

const std::string& Client::getNickName(void) const
{
    return nickname_;
}

const std::string& Client::getUserName(void) const
{
    return username_;
}

const std::string& Client::getHostName(void) const
{
    return hostname_;
}

std::set<std::string>& Client::getChannels(void)
{
    return channels_;
}

bool    Client::getIsAuthenticated(void) const
{
    return is_authenticated_;
}

std::string Client::getHost(void) const
{
    struct sockaddr_in clnt_addr;
    socklen_t size = sizeof(clnt_addr);
    getsockname(client_socket_, &(struct sockaddr& )clnt_addr, &size);

	char *ip = inet_ntoa(clnt_addr.sin_addr);
	std::string result(ip);
	free(ip);

    return result;
}

void Client::setNickName(const std::string& nickname)
{
	nickname_ = nickname;
}

void Client::setUserName(const std::string& username)
{
	username_ = username;
}

void Client::setHostName(const std::string& hostname)
{
	hostname_ = hostname;
}

void Client::addChannel(const std::string& channel)
{
	if (channels_.find(channel) == channels_.end())
		channels_.insert(channel);
}

void Client::deleteChannel(const std::string& channel)
{
	if (channels_.find(channel) != channels_.end())
		channels_.erase(channel);
}

void    Client::setIsAuthenticated(bool is_authenticated)
{
    is_authenticated_ = is_authenticated;
}