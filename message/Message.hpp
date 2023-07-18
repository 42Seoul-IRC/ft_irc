#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class Message {
	public:
		std::string prefix_;
		std::string command_;
		std::vector<std::string> params_;
		std::string trailing_;
		bool hasTrailing_;

		static Message parseMessage(const std::string &str);
};

std::ostream& operator<<(std::ostream& outstream, const Message& msg);

#endif