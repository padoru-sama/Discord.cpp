#pragma once

struct EmbedStructure 
{
	std::wstring title;
	std::wstring type;
	std::wstring description;
	std::wstring url;
	std::wstring timestamp;
	uint32_t color;					// ex : (int a = 0x002F56)
	EmbedFooter footer;
	EmbedImage image;
	EmbedThumbnail thumbnail;
	EmbedVideo video;
	EmbedProvider provider;
	EmbedAuthor author;
	EmbedField field;
};

struct EmbedFooter 
{
	std::wstring text;				//0
	std::wstring icon_url;
	std::wstring proxy_icon_url;
};

struct EmbedImage
{
	std::wstring url;
	std::wstring proxy_url;
	uint32_t height = NULL;
	uint32_t width = NULL;
};

struct EmbedThumbnail
{
	std::wstring url;
	std::wstring proxy_url;
	uint32_t height = NULL;
	uint32_t width = NULL;
};

struct EmbedVideo 
{
	std::wstring url;
	uint32_t height = NULL;
	uint32_t width = NULL; 
};

struct EmbedProvider
{
	std::wstring name;
	std::wstring url;
};

struct EmbedAuthor 
{
	std::wstring name;
	std::wstring url;
	std::wstring icon_url;
	std::wstring proxy_icon_url;
};

struct EmbedField
{
	std::wstring name;		//0
	std::wstring value;		//0
	bool Inline = false;	//?capslock
};