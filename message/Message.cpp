#include "Message.hpp"

Message Message::parseMessage(const std::string &str) 
{
	Message message;
	std::istringstream iss(str);
	std::string params;
	std::string token;

	size_t idx = str.find(":", 1);
	if (idx != std::string::npos)
		message.hasTrailing_ = true;
	if (str[0] == ':') {
		iss.ignore(1);
		std::getline(iss, message.prefix_, ' ');
	}
	std::getline(iss, message.command_, ' ');
	std::getline(iss, params, ':');
	std::istringstream iss_params(params);
	while (iss_params >> token)
		message.params_.push_back(token);
	std::getline(iss, message.trailing_);

	return message;
}

std::ostream& operator<<(std::ostream& outstream, const Message& msg)
{
	outstream << "prefix : " << msg.prefix_ << std::endl;
	outstream << "command : " << msg.command_ << std::endl;
	outstream << "parameters : ";
	for (std::vector<std::string>::const_iterator it = msg.params_.begin(); it != msg.params_.end(); it++) 
	{
		outstream<< *it;
		if (it + 1 != msg.params_.end()) outstream << ", ";
	}
	outstream << std::endl;
	outstream << "trailing : " << msg.trailing_ << std::endl;
	outstream << std::endl;

	return (outstream);
}