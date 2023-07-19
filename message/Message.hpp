#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class Message {
	private:
		std::string prefix_;
		std::string command_;
		std::vector<std::string> params_;
		std::string trailing_;
		bool hasTrailing_;

	public:
		void	makePrefix(std::string& server_name);
		void	makePrefix(Client *client);

		static Message parseMessage(const std::string &str);
		
		void setPrefix (const std::string &prefix);
		void setCommand (const std::string &command);
		void setParams (const std::vector<std::string> &params);
		void setTrailing (const std::string &trailing);
		void setHasTrailing (bool hasTrailing);

		const std::string& getPrefix (void) const;
		const std::string& getCommand (void) const;
		const std::vector<std::string>& getParams (void) const;
		const std::string& getTrailing (void) const;
		bool getHasTrailing (void) const;

		void addParam (const std::string &param);

		std::string toString (void) const;
};

std::ostream& operator<<(std::ostream& outstream, const Message& msg);

#endif