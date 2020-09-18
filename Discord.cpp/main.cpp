#include "discord.h"

//pas touchew, ca sert a rien pour l'instant bruhda
struct snowflake
{

private:
	uint64_t Timestamp;

};

class Bot : public Discord
{
	void onMessage(const std::wstring& msg) override {

		if (msg == std::wstring(L"ouitest"))
		{
			std::cout << "wat" << std::endl;
			sendMessage(L"wat", 539889504866140180);
		}
		//sendmsg("lll", msg.channel);
		//msg.author.id == 
		//sendmsg("oui")
	};
};

int main()
{
	std::wstring input;
	std::cout << "Please, enter your bot token: ";
	std::wcin >> input;

	Bot client;
	client.run(input);
	client.presenceUpdate(Status::online, ActivityStructure(L"418 teapot depuis un websocket", ActivityType::listening));

	printf("send");
	std::cin.ignore(1);
	std::cin.get();
}