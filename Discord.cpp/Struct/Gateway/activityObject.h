#pragma once

//enum

enum class Status { online, notDisturb, idle, invisible, offline };
enum ActivityType { game, streaming, listening, custom = 4 };
enum ActivityFlags { INSTANCE = 1 << 0, JOIN = 1 << 1, SPECTATE = 1 << 2, JOIN_REQUEST = 1 << 3, SYNC = 1 << 4, PLAY = 1 << 5 };

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
	int size[2] = { 0 };
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