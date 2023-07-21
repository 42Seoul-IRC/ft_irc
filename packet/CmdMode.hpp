#pragma once

#include "PacketManager.hpp"
#include "PacketMaker.hpp"

class ModeManager
{
    private:
        char mode_switch;

        std::vector<std::string> *params;
        std::vector<std::string>::iterator it_param;

        std::string changed_buffer; 
        std::string changed_param_buffer;

        Channel *channel_;
        Client *client_;
        PacketMaker *packet_maker_;


    public:
        ModeManager();
        void    setModeSwitch(char mode_switch);

        void    setParams(std::vector<std::string> *params);

        void    setItParam(std::vector<std::string>::iterator it_param);
        bool    isEndParam();
        std::vector<std::string>::iterator getNextParam();
        std::vector<std::string>::iterator getItParam();


        void    pushBackChangedBuffer(std::string buffer);
        void    pushBackChangedParamBuffer(std::string param);

        void    setChannel(Channel *channel);
        void    setClient(Client *client);
        void    setPacketMaker(PacketMaker *packet_maker);



        void    executeMode(char mode);
        bool    canUpdate(char mode);

        void    inviteMode();
        void    topicMode();
        void    keyMode();
        void    opMode();
        void    limitMode();

        void    noMode();
};
