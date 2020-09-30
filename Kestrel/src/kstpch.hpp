/*
 * =====================================================================================
 *
 *       Filename:  kstpch.hpp
 *
 *    Description:  Precompiled header
 *
 *        Version:  1.0
 *        Created:  08/23/2020 03:53:27 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#pragma once

#ifdef _WIN32
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <Windows.h>
#endif

#include <iostream>
#include <memory>
#include <stdexcept>
#include <functional>

#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <string>

#include "Core/Util.hpp"
#include "Debug/Profiler.hpp"
