#pragma once

#include "jsoncpp/json.h"

#ifdef _DEBUG
#ifdef _WIN64
#pragma comment( lib, "jsoncpp/lib/lib_json_x64_debug.lib" )
#else
#pragma comment( lib, "jsoncpp/lib/lib_json_win32_debug.lib" )
#endif // _WIN32
#else
#ifdef _WIN64
#pragma comment( lib, "jsoncpp/lib/lib_json_x64_release.lib" )
#else
#pragma comment( lib, "jsoncpp/lib/lib_json_win32_release.lib" )
#endif // _WIN32
#endif // _DEBUG

