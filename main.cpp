#include "server/Server.hpp"

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage : ./ircserv <port> <password>"<< std::endl;
		return (1);
	}

	try {
		Server server;
		server.init(argv[1], argv[2]);
		server.run();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return (0);
}