#pragma once

#include <kstpch.hpp>

namespace Kestrel {
	using Material = uint32_t;

	static Material loadMaterial( const std::vector<const char*>& shader_paths );
}
