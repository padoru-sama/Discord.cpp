#pragma once
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

//struct
#include "Struct/Gateway/activityObject.h"
/*#include "Struct/Channel/allowedMentionsObject.h"
#include "Struct/Channel/embedObject.h"*/


using namespace std::chrono;
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace web::websockets::client;
using namespace concurrency::streams;       // Asynchronous streams
using namespace utility::conversions;