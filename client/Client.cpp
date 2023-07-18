#include "Client.hpp"

Client::Client(int socket)
{
	client_socket_ = socket;
}

std::string Client::getNickName(void)
{
    return nickname_;
}

std::set<std::string> Client::getChannels(void)
{
    return channels_;
}