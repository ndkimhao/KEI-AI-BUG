#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

//#define USE_PISQPIPE
#define USE_WEBUI

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <ShellAPI.h>
#include <assert.h>
#include <stdexcept>
#include <string>
#include <functional>
#include <map>
#include <iostream>
#include <cstdlib>
#include <list>
#include <future>
#include <chrono>
#include <thread>
#include <sstream>
#include <array>
#include <cstring>
#include <cstdarg>
#include <vector>
#include <utility>
#include <stdexcept>
#include <cstdint>
#include <ostream>
#include <utility>
#include <queue>
#include <climits>
#include <algorithm>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>

std::string Format(const char* format, ...);
