#include "SessionManager.hpp"

Session *SessionManager::getSessionBySocket(int socket)
{
    return (sessions_[session_map_[socket]]);
}

void    SessionManager::addSessionBySocket(int socket)
{
    session_map_[socket] = new Session(socket);   
}

void    SessionManager::removeSessionBySocket(int socket)
{
    delete session_map_[socket];
    session_map_.erase(socket);
}