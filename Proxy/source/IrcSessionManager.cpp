#include "IrcSessionManager.h"
#include "Utils.h"

#include "sstream"


IrcSessionManager::IrcSessionManager()
    : m_cnt(111111)
{
    //ctor
}

IrcSessionManager::~IrcSessionManager()
{
    std::list<IrcSession*>::iterator it;
    for(it = m_list.begin() ; it != m_list.end() ; it++)
        delete *it;
}


IrcSessionManager* IrcSessionManager::instance = NULL;

/** @brief get_instance
  *
  */
IrcSessionManager& IrcSessionManager::get_instance()
{
    if(instance == NULL)
        instance = new IrcSessionManager();
    return(*instance);
}

/** @brief get_session
  *
  */
IrcSession* IrcSessionManager::get_session(std::string id)
{
    IrcSession* ret = NULL;
    if(id == "")
    {
        std::string str;
        std::stringstream ss(str);
        ss << m_cnt++;
        ret = new IrcSession(ss.str());
        m_list.push_back(ret);
        return(ret);
    }
    std::list<IrcSession*>::iterator it;
    for(it = m_list.begin() ; it != m_list.end() ; it++)
    {
        if((*it)->get_id() == id)
        {
            (*it)->stamp();
            return(*it);
        }
    }
    return(ret);
}


IrcSession* IrcSessionManager::get_session_by_nick(std::string nick)
{
    IrcSession* ret = NULL;
    std::list<IrcSession*>::iterator it;
    for(it = m_list.begin() ; it != m_list.end() ; it++)
    {
        if((*it)->get_client().get_user_info().nick == nick)
        {
            return(*it);
        }
    }
    return(ret);
}

void IrcSessionManager::remove_session(std::string id)
{
    std::list<IrcSession*>::iterator it;
    for(it = m_list.begin() ; it != m_list.end() ; it++)
    {
        if((*it)->get_id() == id)
        {
            delete(*it);
            m_list.erase(it);
            break;
        }
    }
}


void IrcSessionManager::check_timeout()
{
    time_t now = time(NULL);
    std::list<IrcSession*>::iterator it;
    for(it = m_list.begin() ; it != m_list.end() ; it++)
    {
        time_t diff = now - (*it)->get_stamp();
        if(diff > Utils::get_timeout())
        {
            (*it)->get_client().quit(Utils::get_timeout_msg());
            remove_session((*it)->get_id());
            it = m_list.begin(); // start over, it is not valid anymore.
        }
    }

}

int IrcSessionManager::get_user_count()
{
    return(m_list.size());
}

std::vector<UserInfo> IrcSessionManager::get_users()
{
    std::vector<UserInfo> v;
    std::list<IrcSession*>::const_iterator it;
    for(it = m_list.begin() ; it != m_list.end() ; it++)
    {
        UserInfo ui = (*it)->get_client().get_user_info();
        v.push_back(ui);
    }
    return(v);
}

