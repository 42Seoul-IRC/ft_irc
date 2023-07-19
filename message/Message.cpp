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
	outstream << "prefix : " << msg.getPrefix() << std::endl;
	outstream << "command : " << msg.getCommand() << std::endl;
	outstream << "parameters : ";
	for (std::vector<std::string>::const_iterator it = msg.getParams().begin(); it != msg.getParams().end(); it++) 
	{
		outstream<< *it;
		if (it + 1 != msg.getParams().end()) outstream << ", ";
	}
	outstream << std::endl;
	outstream << "trailing : " << msg.getTrailing() << std::endl;
	outstream << std::endl;

	return (outstream);
}

void Message::setPrefix(const std::string &prefix)
{
	prefix_ = prefix;
}

void Message::setCommand(const std::string &command)
{
	command_ = command;
}

void Message::setParams(const std::vector<std::string> &params)
{
	params_ = params;
}

void Message::setTrailing(const std::string &trailing)
{
	trailing_ = trailing;
	if (trailing_.size() != 0)
	hasTrailing_ = true;
}

void Message::setHasTrailing (bool hasTrailing)
{
	hasTrailing_ = hasTrailing;
}

const std::string& Message::getPrefix() const
{
	return prefix_;
}

const std::string& Message::getCommand() const
{
	return command_;
}

const std::vector<std::string>& Message::getParams() const
{
	return params_;
}

const std::string& Message::getTrailing() const
{
	return trailing_;
}

bool Message::getHasTrailing() const
{
	return hasTrailing_;
}

void	Message::addParam(const std::string &param)
{
	params_.push_back(param);
}

std::string Message::toString(void) const
{
	std::string str;

	if (prefix_.size() != 0)
		str += ":" + prefix_ + " ";
	str += command_ + " ";
	for (std::vector<std::string>::const_iterator it = params_.begin(); it != params_.end(); it++)
	{
		str += *it;
		if (it + 1 != params_.end())
			str += " ";
	}
	if (hasTrailing_)
		str += " :" + trailing_;
	return (str);
}