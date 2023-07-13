#ifndef SESSION_HPP
# define SESSION_HPP

#include <list>
#include <sstream>

#include "../message/Message.hpp"

class Session {
	private:
		char tmp_[512];
		std::stringstream buffer_;
		std::list<Message> messages_;
		int session_index_;
		int clinet_socket_;
};

#endif