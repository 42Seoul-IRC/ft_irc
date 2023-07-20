NAME = ircserv

CC = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address

SRCS =  channel/Channel.cpp\
		channel/ChannelManager.cpp\
		client/Client.cpp \
		client/ClientManager.cpp \
		message/Message.cpp \
		packet/PacketManager.cpp \
		packet/PacketMaker.cpp \
		packet/Commands.cpp \
		server/Server.cpp \
		socket/Socket.cpp \
		main.cpp
#		packet/CmdMode.cpp \

OBJS = $(SRCS:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o : %.cpp
	$(CC) $(CXXFLAGS) -o $@ -c $<

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re :
	$(MAKE) fclean
	$(MAKE) all

.PHONY : all clean fclean re