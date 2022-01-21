#pragma once

#include "Labyrinth/Core/System/Base.h"

#include <deque>

#ifdef LAB_PLATFORM_WINDOWS
#define _WIN32_WINNT 0x0A00
#endif

//#define ASIO_SEPARATE_COMPILATION

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>