#include "Client.hpp"

Client::Client(int socket)
{
	client_socket_ = socket;
	nickname_ = "";
	username_ = "";
	hostname_ = "";
	is_pass_ = false;
	is_authenticated_ = false;
}

int Client::getSocket(void) const
{
    return client_socket_;
}

std::string Client::getNickName(void)
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

bool	Client::getIsPass(void) const
{
	return is_pass_;
}

std::string Client::getHost(void)
{
    struct sockaddr_in clnt_addr;
    socklen_t size = sizeof(clnt_addr);
    getsockname(client_socket_, &(struct sockaddr& )clnt_addr, &size);

	char *ip = inet_ntoa(clnt_addr.sin_addr);
	std::string result(ip);
	free(ip);

	std::string host_info(getNickName());

	host_info += "!";
	host_info += getUserName();
	host_info += "@";
	host_info += result;

    return host_info;
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

void	Client::setIsPass(bool is_pass)
{
	is_pass_ = is_pass;
}

void	Client::removeClient(void)
{
	nickname_ = "";
	username_ = "";
	hostname_ = "";
	channels_.clear();
	is_authenticated_ = false;
	is_pass_ = false;
	close(client_socket_);
}