#ifndef SESSION_MANAGER_HPP
# define SESSION_MANAGER_HPP

#include <map>
#include <vector>

class Session;

class SessionManager {
	private:
		std::map<int, Session*> session_map_;

	public:
		Session *getSessionBySocket(int socket);
		void	addSessionBySocket(int socket);
		void	removeSessionBySocket(int socket);
};

#endif