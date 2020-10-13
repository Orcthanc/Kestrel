#pragma once

#include <kstpch.hpp>

namespace Kestrel {
	struct Filereader {
		static std::vector<uint8_t> read_uint8t( const std::string& filepath );
		static std::vector<int8_t> read_int8t( const std::string& filepath );
		static std::string read_string( const std::string& filepath );
	};
}
