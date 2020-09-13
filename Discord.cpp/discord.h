#pragma once
#include "Include.h"

class Client
{
	Client(const Client&) = delete;
public:
	Client();
	~Client();


	//fonction

	void run(std::wstring Token);
	void presenceUpdate(Status status, ActivityStructure activity, bool afk, int since);
	void virtual onMessage(std::wstring& msg);


protected:

	std::wstring token;
	websocket_callback_client wssClient;
	milliseconds heartbeat = milliseconds(0);
	uint32_t s = 0;
	std::mutex print;

public:
	std::wstring requestHttp(const method method, const std::wstring& uri);
};



std::wstring Client::requestHttp(const method method, const std::wstring& link)
{

	static http_client httpClient(L"http://discord.com/api");
	http_request req(method);
	req.set_request_uri(link);
	req.headers().add(L"Authorization", L"Bot " + token);

	std::wstring response;
	httpClient.request(req).then([&](http_response msg)
	{
		response = msg.extract_string().get();
	}).wait();

	return response;
	

}





void Client::run(std::wstring arg)
{
	token = arg;


	// connection discord
	wssClient.connect(L"wss://gateway.discord.gg/?v=6&encoding=json").wait();


	//Indentify user in json
	{
		json::value result, d, prop;
		{
			result[L"op"] = 2;
			d[L"token"] = json::value::string(token);
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