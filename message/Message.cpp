#include "Message.hpp"

Message Message::parseMessage(std::string str) 
{
	Message message;
	std::string params;
	std::string token;

	str.erase(str.find_last_not_of(" \n\r\t") + 1);
	str.erase(0, str.find_first_not_of(" \n\r\t"));
	
	std::istringstream iss(str);

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
	std::string temp(param);
	params_.push_back(temp);
}

void	Message::addParam(unsigned long param)
{
	//string stream
	std::stringstream ss(param);
	std::string str;
	
	ss >> str;

	params_.push_back(str);
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

std::vector<std::string> Message::split(std::string str, std::string delimiter)
{
	std::vector<std::string> result;
	size_t pos = 0;
	std::string token;

	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		token = str.substr(0, pos);
		result.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	result.push_back(str);
	return (result);
}

void Message::clear(void)
{
	prefix_.clear();
	command_.clear();
	params_.clear();
	trailing_.clear();
	hasTrailing_ = false;
}