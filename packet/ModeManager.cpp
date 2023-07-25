#include "ModeManager.hpp"
#include "PacketManager.hpp"

ModeManager::ModeManager()
{
    mode_switch = 0;
    changed_mode_buffer = "";
}

ModeManager::~ModeManager()
{
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
    if (mode == 'o')
    {
        if (mode_switch == '+' && !channel_->checkClientIsOperator(client_->getNick()))
            return true;
        else if (mode_switch == '-' && channel_->checkClientIsOperator(client_->getNick()))
            return true;
        else
            return false;
    }

    if (!channel_->isOnChannelMode(mode) && mode_switch == '+' )
        return true;
    else if (channel_->isOnChannelMode(mode) && mode_switch == '-' )
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

bool    ModeManager::isEndItParam()
{
    if (it_param == params->end())
        return true;
    else
        return false;
}

std::vector<std::string>::iterator ModeManager::getNextItParam()
{
    return it_param++;
}

std::vector<std::string>::iterator ModeManager::getItParam()
{
    return it_param;
}

void    ModeManager::incrementItParam()
{
    it_param++;
}

void    ModeManager::pushBackChangedBuffer(std::string buffer)
{
    // changed_mode_buffer의 맨 마지막 부터 +나 -를 탐색한다. 
    // 없으면, 그냥 추가한다.
    // +가 있는데, mode_switch가 +이면, 그냥 추가한다.
    // +가 있는데, mode_switch가 -이면, -를 추가한다. 
    // -가 있는데, mode_switch가 +이면, +를 추가한다.
    // -가 있는데, mode_switch가 -이면, 그냥 추가한다.

    // 뒤에서 부터 한 글자씩, 확인하며 +나 -가 있는지 확인하는 if문 작성해줘
    int i = changed_mode_buffer.size() - 1;
    for (; i >= 0; i--)
    {
        if (changed_mode_buffer[i] == '+')
        {
            if (mode_switch == '+')
            {
                break;
            }
            else if (mode_switch == '-')
            {
                changed_mode_buffer += mode_switch;
                break;  
            }
        }
        if (changed_mode_buffer[i] == '-')
        {
            if (mode_switch == '+')
            {
                changed_mode_buffer += mode_switch;
                break;
            }
            else if (mode_switch == '-')
            {
                break;
            }
        }
    }

    if (i == -1)
        changed_mode_buffer += mode_switch;
    changed_mode_buffer += buffer;
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

void    ModeManager::sendSuccessMsg()
{
    packet_maker_->BroadcastMode(packet, changed_mode_buffer, changed_param_buffer);
}

//modeisgchanged

bool    ModeManager::modeIsChanged()
{
    if (changed_mode_buffer.size() != 0)
        return true;
    else
        return false;
}

std::string ModeManager::makeCurModeStatus()
{
    std::string cur_mode_status = "";
    cur_mode_status += '+';;
    cur_mode_status += channel_->mode_;
    //check key mode is on?
    if (channel_->isOnChannelMode('k'))
    {
    cur_mode_status += " ";
        std::stringstream ss;
        ss << channel_->password_;
        cur_mode_status += ss.str();
    }
    if (channel_->isOnChannelMode('l'))
    {
        cur_mode_status += " ";
        std::stringstream ss;
        ss << channel_->limit_;
        cur_mode_status += ss.str();
    }
    return cur_mode_status;
}

void    ModeManager::setPacket(struct Packet packet)
{
    this->packet = packet;
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
    if (mode_switch == '+')
    {
        if (it_param == params->end())
        {
            packet_maker_->ErrNeedMoreParamsKey(packet);
            return ;
        }
        
        std::string key = *it_param;

        //check space in key
        if (key.find(' ') != std::string::npos)
        {
            //ERR_INVAlid key 525

            return ;
        }

        channel_->setChannelMode('k');
        channel_->setPassword(key);
        
        pushBackChangedBuffer("k");
        pushBackChangedParamBuffer(key);

        incrementItParam();
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
        packet_maker_->ErrNeedMoreParamsOp(packet);
        return ;
    }

    std::string nick = *it_param;
    if (!channel_->checkClientIsInChannel(nick))
    {
        // ERR_USERNOTINCHANNEL (441)
        packet_maker_->ErrUserNotInChannel(packet);
        return ;
    }

    if (mode_switch == '+')
    {
        // channel_->setChannelMode('o');
        channel_->setOperator(nick);

        pushBackChangedBuffer("o");
        pushBackChangedParamBuffer(nick);   
    }
    else if (mode_switch == '-')
    {
        // channel_->unsetChannelMode('o');
        channel_->unsetOperator(nick);

        pushBackChangedBuffer("o");
        pushBackChangedParamBuffer(nick);   
    }
    incrementItParam();

    
}

void    ModeManager::limitMode()
{
    if (mode_switch == '+')
    {
        if (it_param == params->end())
        {
            packet_maker_->ErrNeedMoreParamsLimit(packet);
            return ;
        }
 
        std::string limit = *it_param; 
        std::stringstream ss;
        
        ss << (limit);

        int limit_int;
        ss >> limit_int;

        if (limit_int < 0 || ss.fail() || !ss.eof()) 
        {
            // error param is not number
            // 696
            packet_maker_->ErrNeedMoreParamsLimit(packet);
            return ;
        }
        //limit is n
        
        channel_->setChannelMode('l');
        channel_->setLimit(limit_int);

        pushBackChangedBuffer("l");
        pushBackChangedParamBuffer(limit);

        incrementItParam();
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
    //check mode is invalid
    
    if (mode != 'i' && mode != 't' && mode != 'k' && mode != 'l' && mode != 'o')
    {
        if (mode == 'n')
            return ;
        
        packet_maker_->ErrUnknownMode(packet, mode);
        return ;
    }

    if (!canUpdate(mode))
        return ;

    if (mode == 'i')
        inviteMode();
    else if (mode == 't')
        topicMode();
    else if (mode == 'k')
        keyMode();
    else if (mode == 'o')
        opMode();
    else if (mode == 'l')
        limitMode();
}

void	PacketManager::mode(struct Packet& packet)
{
    std::string client_nick = getNickBySocket(packet.client_socket);
	Client *client = client_manager_.getClientByNick(client_nick);


    //check channel error 
    //use iterator for params
	
    ModeManager mode_manager;

    std::vector<std::string> params = packet.message.getParams();
    mode_manager.setParams(&params);
    mode_manager.setItParam(params.begin());
    

    //check params[0] exist?
    if (mode_manager.getItParam() == params.end())
    {
        // ERR_NEEDMOREPARAMS (461)
        packet_maker_->ErrNeedMoreParams(packet);
        return ;
    }


    std::string channel_name = *mode_manager.getItParam();
    Channel *channel = channel_manager_.getChannelByName(channel_name);
    mode_manager.incrementItParam();
    mode_manager.setChannel(channel);

    
    // check if channel start with #
    if (channel_name[0] != '#')
    {
        return ;
    }

    //check there is no chnnel in server
    if (channel == NULL)
    {
        // ERR_NOSUCHCHANNEL (403)
        packet_maker_->ErrNoSuchChannel(packet);
        return ;
    }

  

    //check client is in channel
    // ERR_NOTONCHANNEL (442)
    if (!channel->checkClientIsInChannel(client_nick))
    {
        packet_maker_->ErrNotOnChannel(packet);
        return ;
    }

    // check if params[1] == mode string is empty, send channel mode
    // RPL_CHANNELMODEIS (324)
    // RPL_CREATIONTIME (329)
    if (mode_manager.isEndItParam())
    {
        packet_maker_->RplChannelModeIs(packet, channel, mode_manager.makeCurModeStatus());
        packet_maker_->RplCreationTime(packet, channel);
        return ;
    }

    //check user is operator of channel
    // ERR_CHANOPRIVSNEEDED (482)
    if (!channel->checkClientIsOperator(client_nick))
    {
        packet_maker_->ErrChanOPrivsNeeded(packet);
        return ;
    }
    mode_manager.setClient(client);
    mode_manager.setPacketMaker(packet_maker_);
    mode_manager.setPacket(packet);


	//business logic

    std::string mode_string = *mode_manager.getItParam();
    mode_manager.incrementItParam();
    std::stringstream ss;
    
    ss << mode_string;
    char mode;
    while (ss >> mode)
    {

        if (mode == '+' || mode == '-')
        {
            mode_manager.setModeSwitch(mode);
            continue;
        }
        mode_manager.executeMode(mode);
        // mode_manager.printMode();
    }
    if (mode_manager.modeIsChanged())
        mode_manager.sendSuccessMsg();
}

void    ModeManager::printMode()
{
    std::cout << "mode_switch: " << mode_switch << std::endl;
    std::cout << "current_mode: " << channel_->mode_ << std::endl;
    std::cout << "changed_mode_buffer: " << changed_mode_buffer << std::endl;
    std::cout << "changed_param_buffer: " << changed_param_buffer << std::endl;
}