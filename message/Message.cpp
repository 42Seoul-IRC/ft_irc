#include "Message.hpp"

Message Message::parse(const std::string &str)
{
	Message message;

	message.parseMessage(str);

	return message;
}

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