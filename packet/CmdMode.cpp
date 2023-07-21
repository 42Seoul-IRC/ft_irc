#include "PacketManager.hpp"
#include "ModeManager.hpp"

ModeManager::ModeManager()
{
    mode_switch = 0;
    changed_mode_buffer = "";
}

void    ModeManager::setModeSwitch(char mode_switch)
{
    this->mode_switch = mode_switch;
}

//Can update mode??
bool    ModeManager::canUpdate(char mode)
{
    //check when mode_switch is +, there don't exists mode
    //check when mode_switch is -, there exists mode
    
    if (channel_->isOnChannelMode(mode) && mode_switch == '+' )
        return true;
    else if (!channel_->isOnChannelMode(mode) && mode_switch == '-' )
        return true;
    else
        return false;
}

void    ModeManager::setParams(std::vector<std::string> *params)
{
    this->params = params;
}

void    ModeManager::setItParam(std::vector<std::string>::iterator it_param)
{
    this->it_param = it_param;
}

bool    ModeManager::isEndParam()
{
    if (it_param == params->end())
        return true;
    else
        return false;
}

std::vector<std::string>::iterator ModeManager::getNextParam()
{
    return it_param++;
}

std::vector<std::string>::iterator ModeManager::getItParam()
{
    return it_param;
}

void    ModeManager::pushBackChangedBuffer(std::string buffer)
{
    if (changed_mode_buffer.size() != 0)
        changed_mode_buffer += " ";
    changed_mode_buffer += buffer
}

void    ModeManager::pushBackChangedParamBuffer(std::string param)
{
    //if buffer size is not 0, add space
    if (changed_param_buffer.size() != 0)
        changed_param_buffer += " ";
    changed_param_buffer += param;
}

void    ModeManager::setChannel(Channel *channel)
{
    this->channel_ = channel;
}

void    ModeManager::setClient(Client *client)
{
    this->client_ = client;
}

void    ModeManager::setPacketMaker(PacketMaker *packet_maker)
{
    this->packet_maker_ = packet_maker;
}


void    ModeManager::inviteMode()
{
    if (mode_switch == '+')
    {
        channel_->setChannelMode('i');
        pushBackChangedBuffer("i");
    }
    else if (mode_switch == '-')
    {
        channel_->unsetChannelMode('i');
        pushBackChangedBuffer("i");
    }
}

void    ModeManager::topicMode()
{
    if (mode_switch == '+')
    {
        channel_->setChannelMode('t');
        pushBackChangedBuffer("t");
    }
    else if (mode_switch == '-')
    {
        channel_->unsetChannelMode('t');
        pushBackChangedBuffer("t");
    }
}

void    ModeManager::keyMode()
{    
    it_param++;
    
    if (mode_switch == '+')
    {
        if (it_param == params->end())
        {
            // ERR_NEEDMOREPARAMS (461)
            return ;
        }
        std::string key = *it_param;

        //check space in key
        if (key.find(' ') != std::string::npos)
        {
            //ERR_INVAlid key 525

            return ;
        }
        //key is n

        channel_->setChannelMode('k');
        channel_->setPassword(key);
        
        pushBackChangedBuffer("k");
        pushBackChangedParamBuffer(key);
    }
    else if (mode_switch == '-')
    {
        channel_->unsetChannelMode('k');
        channel_->clearPassword();
        
        pushBackChangedBuffer("k");
    }
}

void    ModeManager::opMode()
{
    if (it_param == params->end())
    {
        // ERR_NEEDMOREPARAMS (461)
        return ;
    }

    std::string nick = *it_param;
    if (!channel_->checkClientIsInChannel(nick))
    {
        // ERR_USERNOTINCHANNEL (441)
        return ;
    }

    if (mode_switch == '+')
    {
        channel_->setOperator(nick);

        pushBackChangedBuffer("o");
        pushBackChangedParamBuffer(nick);   
    }
    else if (mode_switch == '-')
    {
        channel_->unsetOperator(nick);

        pushBackChangedBuffer("o");
        pushBackChangedParamBuffer(nick);   
    }

}

void    ModeManager::limitMode()
{
    it_param++;
    if (mode_switch == '+')
    {
        if (it_param == params->end())
        {
            // ERR_NEEDMOREPARAMS (461)
            return ;
        }
        
        std::string limit = *it_param; 
        std::stringstream ss(limit);

        int limit_int;
        ss >> limit_int;

        if (limit_int < 0 || ss.fail() || !ss.eof()) 
        {
            // error param is not number
            // ERR_NEEDMOREPARAMS (461)
            return ;
        }
        //limit is n

        channel_->setChannelMode('l');
        channel_->setLimit(std::stoi(limit));
        
        pushBackChangedBuffer("l");
        pushBackChangedParamBuffer(limit);
    }
    else if (mode_switch == '-')
    {
        channel_->unsetChannelMode('l');
        channel_->setLimit(-1);
        
        pushBackChangedBuffer("l");
    }
}



void    ModeManager::executeMode(char mode)
{
    //only handle i t k o l
    
    if (mode == 'i' && canUpdate(mode))
        inviteMode();
    else if (mode == 't' && canUpdate(mode))
        topicMode();
    else if (mode == 'k' && canUpdate(mode))
        keyMode();
    else if (mode == 'o' && canUpdate(mode))
        opMode();
    else if (mode == 'l' && canUpdate(mode))
        limitMode();
    else 
        noMode();
}

void    ModeManager::noMode()
{
    //send error
}

void	PacketManager::mode(struct Packet& packet)
{
    std::string client_nick = getNickBySocket(packet.client_socket);
	Client *client = client_manager_.getClientByNick(client_nick);


    //check channel error 
    //use iterator for params
	
    ModeManager mode_manager;
    PacketMaker packet_maker;

    std::vector<std::string> params = packet.message.getParams();
    mode_manager.setParams(&params);
    mode_manager.setItParam(params.begin());
    

    if (mode_manager.getItParam() == params.end())
    {
        // ERR_NEEDMOREPARAMS (461)
        packet_maker.ErrNeedMoreParams(packet);
        return ;
    }
    std::string channel_name = *mode_manager.getItParam();
    Channel *channel = channel_manager_.getChannelByName(channel_name);
    
    
    //check error no such channel
    if (channel == NULL)
    {
        // ERR_NOSUCHCHANNEL (403)
        packet_maker.ErrNoSuchChannel(packet);
        return ;
    }

    mode_manager.setChannel(channel);
    mode_manager.setClient(client);
    mode_manager.setPacketMaker(&packet_maker);

    // if params[1] is empty, send channel mode
    // RPL_CHANNELMODEIS (324)
    // RPL_CREATIONTIME (329)
    if (mode_manager.isEndParam())
    {
        packet_maker.RplChannelModeIs(packet, channel);
        packet_maker.RplCreationTime(packet, channel);
        return ;
    }

    //check user is operator of channel
    // ERR_CHANOPRIVSNEEDED (482)
    if (!channel->checkClientIsOperator(client_nick))
    {
        packet_maker.ErrChanOPrivsNeeded(packet);
        return ;
    }


	//business logic


    std::stringstream ss(packet.message.getParams()[1]);
    char mode;

    while (ss >> mode)
    {
        if (mode == '+' || mode == '-')
        {
            mode_manager.setModeSwitch(mode);
            continue;
        }
        mode_manager.executeMode(mode);
    }

}
