#pragma once
#include "discord.h"

void Discord::sendMessage(const std::wstring& content, const uint64_t channel, bool tts)
{
	json::value result;
	{
		result[L"content"] = json::value::string(content);
		//result[L"nonce"] = nonce;
		result[L"tts"] = json::value::boolean(tts);
		std::wcout << "\n\n\n" << result.serialize() << "\n\n\n";
		
	}
	requestHttp(methods::POST, L"/channels/" + std::to_wstring(channel) + L"/messages", result);
}