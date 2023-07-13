#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <string>
#include <vector>

class Message {
	private:
		std::string prefix_;
		std::string command_;
		std::vector<std::string> params_;
		std::string trailing_;
};

#endif