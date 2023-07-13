#ifndef SESSION_MANAGER_HPP
# define SESSION_MANAGER_HPP

#include <map>
#include <vector>

class Session;

class SessionManager {
	private:
		std::map<int, int> session_index_map_;
		std::vector<Session*> sessions_;
		std::vector<int> session_index_pool_;
};

#endif