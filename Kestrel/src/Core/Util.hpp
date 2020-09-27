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

#include <kstpch.hpp>

#include "Core/Log.hpp"

#include <type_traits>

#ifndef NDEBUG
#define KST_CORE_ASSERT( val, ... )											\
	if( !val ){																\
		KST_CORE_CRITICAL( __VA_ARGS__ );									\
		throw std::runtime_error( fmt::format( __VA_ARGS__ ));				\
	}

#define KST_ASSERT( val, ... )												\
	if( !val ){																\
		KST_CRITICAL( __VA_ARGS__ );										\
		throw std::runtime_error( fmt::format( __VA_ARGS__ ));				\
	}
#else
#define KST_CORE_ASSERT( ... )
#define KST_ASSERT( ... )
#endif

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 1


namespace Kestrel {
	template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
	E operator&( const E& lhs, const E& rhs ){
		return static_cast<E>( static_cast<std::underlying_type_t<E>>( lhs ) & static_cast<std::underlying_type_t<E>>( rhs ));
	}
	template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
	E operator^( const E& lhs, const E& rhs ){
		return static_cast<E>( static_cast<std::underlying_type_t<E>>( lhs ) ^ static_cast<std::underlying_type_t<E>>( rhs ));
	}
	template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
	E operator|( const E& lhs, const E& rhs ){
		return static_cast<E>( static_cast<std::underlying_type_t<E>>( lhs ) | static_cast<std::underlying_type_t<E>>( rhs ));
	}

	template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
	E& operator&=( const E& lhs, const E& rhs ){
		return lhs = static_cast<E>( static_cast<std::underlying_type_t<E>>( lhs ) & static_cast<std::underlying_type_t<E>>( rhs ));
	}
	template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
	E& operator^=( const E& lhs, const E& rhs ){
		return lhs = static_cast<E>( static_cast<std::underlying_type_t<E>>( lhs ) ^ static_cast<std::underlying_type_t<E>>( rhs ));
	}
	template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
	E& operator|=( const E& lhs, const E& rhs ){
		return lhs = static_cast<E>( static_cast<std::underlying_type_t<E>>( lhs ) | static_cast<std::underlying_type_t<E>>( rhs ));
	}
}
