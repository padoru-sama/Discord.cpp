#include "discord.h"

Client::Client()
{

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
			if (data[L"t"].as_string() == L"MESSAGE_CREATE")
			{
																											std::wcout << data[L"d"].as_object()[L"author"].as_object()[L"username"].as_string() << " a dit " << data[L"d"].as_object()[L"content"].as_string() << "\n\n";
				std::wstring msg = data[L"d"].as_object()[L"content"].as_string();
				onMessage(msg);
			}
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
	wssClient.close().wait();
}

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