

#include "PacketManager.hpp"

class ModeManager
{
    private:
        char mode_switch;
        std::string changed_buffer; 


    public:
        ModeManager();
        void    setModeSwitch(char mode_switch);

        void    executeMode(char mode);

        void    inviteMode();
        void    topicMode();
        void    keyMode();
        void    opMode();
        void    limitMode();
};

ModeManager::ModeManager()
{
    mode_switch = 0;
    changed_buffer = "";
}

void    ModeManager::setModeSwitch(char mode_switch)
{
    this->mode_switch = mode_switch;
}

void    ModeManager::executeMode(char mode)
{
    //only handle i t k o l
    
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
	Client * client = client_manager_.getClientByNick(client_nick);

	std::string channel_name = packet.message.getParams()[0];
	Channel *channel = channel_manager_.getChannelByName(channel_name);

	std::string mode_str = packet.message.getParams()[1];
	std::string mode_par = packet.message.getParams()[2];

    ModeManager mode_manager;


	//business logic


	//mode handling
    std::stringstream ss(mode_str);
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
