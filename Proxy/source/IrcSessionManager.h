#ifndef IRCSESSIONMANAGER_H
#define IRCSESSIONMANAGER_H

#include "IrcSession.h"


#include <list>


class IrcSessionManager
{
public:
    virtual ~IrcSessionManager();

    IrcSession* get_session(std::string id = "");
    IrcSession* get_session_by_nick(std::string nick);
    int get_user_count();
    std::vector<UserInfo> get_users();
    void remove_session(std::string id);
    void check_timeout();

    static IrcSessionManager& get_instance();

private:
    IrcSessionManager();
    IrcSessionManager(const IrcSessionManager& rhs){};
    std::list<IrcSession*> m_list;
    unsigned int m_cnt;
    static IrcSessionManager* instance;
};

#endif // IRCSESSIONMANAGER_H
