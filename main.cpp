#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>              // HTTP server
#include <cpprest/json.h>                       // JSON library
#include <cpprest/uri.h>                        // URI library
#include <cpprest/containerstream.h>            // Async streams backed by STL containers
#include <cpprest/interopstream.h>              // Bridges for integrating Async streams with STL and WinRT streams
#include <cpprest/rawptrstream.h>               // Async streams backed by raw pointer to memory
#include <cpprest/producerconsumerstream.h>     // Async streams for producer consumer scenarios
#include <cpprest/ws_client.h>					//websocket
#include <cpprest/asyncrt_utils.h>				//conversion utf8
#include <string>
#include <thread>
#include <mutex>
#include <chrono>


using namespace std::chrono;
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace web::websockets::client;
using namespace concurrency::streams;       // Asynchronous streams
using namespace utility::conversions;

//pas touchew, ca sert a rien pour l'instant bruhda
struct snowflake
{

private:
	uint64_t Timestamp;
	
};


class Client
{
	Client(const Client&) = delete;
public:
	Client();
	~Client();

	//enum

	enum class Status { online, notDisturb, idle, invisible, offline };
	enum ActivityType { game, streaming, listening, custom = 4 };
	enum ActivityFlags { INSTANCE = 1, JOIN = 2, SPECTATE = 4, JOIN_REQUEST = 8, SYNC = 16, PLAY = 23 };

	//struct

	struct ActivityTimestamps
	{
		int start = NULL;
		int end = NULL;
	};

	struct ActivityEmoji
	{
		std::wstring name = L""; //0
		std::wstring id;
		bool animated = false;
	};

	struct ActivityParty
	{
		std::wstring id;
		int size[2] = {0};
	};

	struct ActivityAssets
	{
		std::wstring large_image;
		std::wstring large_text;
		std::wstring small_image;
		std::wstring small_text;
	};

	struct ActivitySecrets
	{
		std::wstring join;
		std::wstring spectate;
		std::wstring match;
	};

	struct ActivityStructure //Game in json
	{
		std::wstring name;                //0
		ActivityType type;                //0
		std::wstring url;
		int created_at;                   //0
		ActivityTimestamps timestamps;
		std::wstring application_id;
		std::wstring details;
		std::wstring state;
		ActivityEmoji emoji;
		ActivityParty party;
		ActivityAssets assets;
		ActivitySecrets secrets;
		bool instance = false;
		int flags = NULL;
		ActivityStructure(std::wstring name, ActivityType type = ActivityType::game, int created_at = time(nullptr)) : name(name), type(type), created_at(created_at) {};
	};



	//fonction

	void run(std::wstring Token);
	void presenceUpdate(Status status, ActivityStructure activity, bool afk, int since);



	
protected:
	std::string token;
	http_client* httpClient;
	websocket_callback_client wssClient;
	milliseconds heartbeat = milliseconds(0);
	uint32_t s = 0;
	std::mutex print;
};

Client::Client()
{
	httpClient = new http_client(L"https://discord.com/api");
	


	// affichage du msg websocket ressu + heartbeat
	wssClient.set_message_handler([&](websocket_incoming_message msg)
	{
		print.lock();
		json::value data = json::value::parse(msg.extract_string().get());

		switch (data[L"op"].as_integer())
		{
		case 0:
			s = data[L"s"].as_integer();
			std::wcout << "on a recu un event" << '\n';
			break;
		case 1:
			std::cout << "On a recu un event chef" << '\n';
			break;
		case 7:
			std::cout << "on doit se reco" << '\n';
			break;
		case 9:
			std::cout << "invalid cession pute" << '\n';
			break;
		case 10:
			heartbeat = milliseconds(data[L"d"].as_object()[L"heartbeat_interval"].as_integer());
			std::cout << "heartbeat vaut " << heartbeat.count() << '\n';
			break;
		case 11:
			std::cout << "heartbeat recu par le server" << '\n';
			break;
		default:
			std::cout << "OP CODE NON PRIS EN COMPTE" << '\n';
		}
		print.unlock();

	});
}

Client::~Client()
{
	delete httpClient;
	wssClient.close().wait();
}



void Client::run(std::wstring Token)
{



	// connection discord
	wssClient.connect(L"wss://gateway.discord.gg/?v=6&encoding=json").wait();


	//Indentify user in json
	{
		json::value result, d, prop;
		{
			result[L"op"] = 2;
			d[L"token"] = json::value::string(Token);
			prop[L"$os"] = json::value::string(L"Windows");
			prop[L"$browser"] = json::value::string(L"cpprestsdk");
			prop[L"$device"] = json::value::string(L"cpprestsdk");
			d[L"properties"] = json::value::value(prop);
			result[L"d"] = json::value::value(d);
		}

		websocket_outgoing_message msg;
		msg.set_utf8_message(to_utf8string(result.serialize()));
		wssClient.send(msg).then([&] {
			print.lock();
			std::cout << "connected." << "\n";
			print.unlock();
		}).wait();
	}

	std::thread([&]
	{
		json::value result;
		result[L"op"] = 1;
		websocket_outgoing_message msg;
		while (true)
		{
			result[L"d"] = s ? s != 0 : result[L"d"] = json::value::null();
			msg.set_utf8_message(to_utf8string(result.serialize()));


			print.lock();
			std::wcout << "S vaut: " << s << " et Hearthbeat vaut:" << result.serialize() << '\n';
			print.unlock();
			while (heartbeat.count() == 0)
				std::this_thread::sleep_for(1s);
			wssClient.send(msg).then([&]
			{

				print.lock();
				std::cout << "45s fini" << "\n";
				print.unlock();
			});
			std::this_thread::sleep_for(heartbeat);
		}
	}).detach();
}


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






int main()
{		
	
	Client client;
	client.run(TOKEN);
	
	client.presenceUpdate(Client::Status::online, Client::ActivityStructure(L"418 teapot", Client::ActivityType::listening));
	std::cin.get();
}