#pragma once

#include "PacketManager.hpp"
#include "PacketMaker.hpp"

class ModeManager
{
    private:
        char mode_switch;

        std::vector<std::string> *params;
        std::vector<std::string>::iterator it_param;

        std::string changed_mode_buffer; 
        std::string changed_param_buffer;

        Channel *channel_;
        Client *client_;
        PacketMaker *packet_maker_;
        struct Packet packet;


    public:
        ModeManager();
		~ModeManager();
        void    setModeSwitch(char mode_switch);

        void    setParams(std::vector<std::string> *params);

        void    setItParam(std::vector<std::string>::iterator it_param);
        bool    isEndItParam();
        std::vector<std::string>::iterator getNextItParam();
        std::vector<std::string>::iterator getItParam();
        void    incrementItParam();


        void    pushBackChangedBuffer(std::string buffer);
        void    pushBackChangedParamBuffer(std::string param);

        void    setChannel(Channel *channel);
        void    setClient(Client *client);
        void    setPacketMaker(PacketMaker *packet_maker);
        void    setPacket(struct Packet packet);

        void    sendSuccessMsg();
        std::string makeCurModeStatus();


        void    executeMode(char mode);
        bool    canUpdate(char mode);

        void    inviteMode();
        void    topicMode();
        void    keyMode();
        void    opMode();
        void    limitMode();

        void    printMode();

        bool    modeIsChanged();
};
