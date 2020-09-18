#pragma once

//enum
enum class AllowedMentionTypes{roles, users, everyone};


struct AllowedMentions {
	AllowedMentionTypes type[3];
	std::wstring roles[100];
	std::wstring users[100];
};