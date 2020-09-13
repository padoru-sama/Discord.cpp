#include "discord.h"



//pas touchew, ca sert a rien pour l'instant bruhda
struct snowflake
{

private:
	uint64_t Timestamp;

};








void Client::presenceUpdate(Status status, ActivityStructure activity, bool afk = false, int since = NULL)
{
	// Presence
	{

		json::value timestamps;
		{
			if (activity.timestamps.start)
				timestamps[L"start"] = activity.timestamps.start;
			if (activity.timestamps.end)
				timestamps[L"end"] = activity.timestamps.end;
		}

		json::value emoji;

		{
			emoji[L"name"] = json::value::string(activity.emoji.name);

			if (!(activity.emoji.id.empty()))
				emoji[L"id"] = json::value::string(activity.emoji.id);

			if (!(activity.emoji.animated))
				emoji[L"animated"] = json::value::boolean(activity.emoji.animated);
		}

		json::value party;
		{

			if (!(activity.party.id.empty()))
				party[L"id"] = json::value::string(activity.party.id);

			if (!(activity.party.size[0] == NULL && activity.party.size[1] == NULL))
			{
				party[L"size"][0] = activity.party.size[0];
				party[L"size"][1] = activity.party.size[1];
			}

		}

		json::value assets;
		{
			if (!(activity.assets.large_image.empty()))
				assets[L"large_image"] = json::value::string(activity.assets.large_image);
			if (!(activity.assets.large_text.empty()))
				assets[L"large_text"] = json::value::string(activity.assets.large_text);
			if (!(activity.assets.small_image.empty()))
				assets[L"small_image"] = json::value::string(activity.assets.small_image);
			if (!(activity.assets.small_text.empty()))
				assets[L"small_text"] = json::value::string(activity.assets.small_text);
		}

		json::value secrets;
		{
			if (!(activity.secrets.join.empty()))
				secrets[L"join"] = json::value::string(activity.secrets.join);
			if (!(activity.secrets.spectate.empty()))
				secrets[L"spectate"] = json::value::string(activity.secrets.spectate);
			if (!(activity.secrets.match.empty()))
				secrets[L"match"] = json::value::string(activity.secrets.match);
		}


		json::value game;
		{
			game[L"name"] = json::value::string(activity.name);
			game[L"type"] = activity.type;
			game[L"created_at"] = activity.created_at;
			if (!(activity.url.empty()))
				game[L"url"] = json::value::string(activity.url);
			if (timestamps.has_field(L"start") || timestamps.has_field(L"end"))
				game[L"timestamps"] = json::value::value(timestamps);
			if (!(activity.application_id.empty()))
				game[L"application_id"] = json::value::string(activity.application_id);
			if (!(activity.details.empty()))
				game[L"details"] = json::value::string(activity.details);
			if (!(activity.state.empty()))
				game[L"state"] = json::value::string(activity.state);

			if (!(activity.emoji.name.empty()))
				game[L"emoji"] = json::value::value(emoji);

			if (party.has_field(L"id") || party.has_field(L"size"))
				game[L"party"] = json::value::value(party);

			if (assets.has_field(L"large_image") || assets.has_field(L"large_text") || assets.has_field(L"small_image") || assets.has_field(L"small_text"))
				game[L"assets"] = json::value::value(assets);

			if (secrets.has_field(L"join") || secrets.has_field(L"spectate") || secrets.has_field(L"match"))
				game[L"secrets"] = json::value::value(secrets);

			if (activity.instance)
				game[L"instance"] = json::value::boolean(activity.instance);

			if (activity.flags)
				game[L"flags"] = activity.flags;
		}

		json::value result, d;
		{

			since == NULL ? d[L"since"] = json::value::null() : d[L"since"] = since;
			d[L"afk"] = json::value::boolean(since);
			d[L"game"] = json::value::value(game);
			switch (status)
			{
			case Status::online:
				d[L"status"] = json::value::string(L"online");
				break;
			case Status::notDisturb:
				d[L"status"] = json::value::string(L"dnd");
				break;
			case Status::idle:
				d[L"status"] = json::value::string(L"idle");
				break;
			case Status::invisible:
				d[L"status"] = json::value::string(L"invisible");
				break;
			case Status::offline:
				d[L"status"] = json::value::string(L"offline");
				break;
			}
			result[L"d"] = json::value::value(d);
			result[L"op"] = 3;

		}
		std::wcout << "\n\n\n\n" << result.serialize() << '\n';
		//send update status
		{
			websocket_outgoing_message msg;
			msg.set_utf8_message(to_utf8string(result.serialize()));
			wssClient.send(msg).then([&]
			{
				print.lock();
				std::cout << "status updated." << "\n";
				print.unlock();
			}).wait();
		}
	}
}

void Client::onMessage(std::wstring& msg)
{}







class Bot : public Client
{
	void onMessage(std::wstring& msg) override {

		if (msg == std::wstring(L"ouitest"))
		{
			std::cout << "wat" << std::endl;
		}
		//sendmsg("lll", msg.chanell);
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

	//GET / "channels /538839396540219393";
	std::wcout << client.requestHttp(methods::GET, L"channels/538839396540219393") << "\n\n\n\n";

	std::cin.ignore(1);
	std::cin.get();
}