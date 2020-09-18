#pragma once
#include "include.h"

class Discord
{
	Discord(const Discord&) = delete;
public:
	Discord();
	~Discord();


	//fonction

	
	void run(std::wstring Token);
	void sendMessage(const std::wstring& content, const uint64_t channel, bool tts = false);

	//events
	void presenceUpdate(Status status, ActivityStructure activity, bool afk = false, int since = NULL);
	void virtual onMessage(const std::wstring&) {}

protected:

	std::wstring token;
	websocket_callback_client wssClient;
	milliseconds heartbeat = milliseconds(0);
	uint32_t s = 0;
	std::mutex print;

public:
	std::wstring requestHttp(const method method, const std::wstring& link, const json::value& params = json::value());
};
