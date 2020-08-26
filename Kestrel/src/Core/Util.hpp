/*
 * =====================================================================================
 *
 *       Filename:  Util.hpp
 *
 *    Description:  Some useful defines
 *
 *        Version:  1.0
 *        Created:  08/26/2020 02:54:24 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include "kstpch.hpp"
#include "Log.hpp"

#ifndef NDEBUG
#define KST_CORE_ASSERT( val, ... )											\
	if( !val ){																\
		KST_CORE_CRITICAL( __VA_ARGS__ );									\
		throw std::runtime_error( fmt::format( __VA_ARGS__ ));				\
	}

#define KST_ASSERT( val, ... )											\
	if( !val ){																\
		KST_CRITICAL( __VA_ARGS__ );									\
		throw std::runtime_error( fmt::format( __VA_ARGS__ ));				\
	}
#else
#define KST_CORE_ASSERT( ... )
#define KST_ASSERT( ... )
#endif
