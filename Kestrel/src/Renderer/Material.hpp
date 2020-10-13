#pragma once

#include <kstpch.hpp>

namespace Kestrel {
	using Material = uint32_t;

	struct Materials {
		virtual ~Materials() = default;
		virtual Material loadMaterial( const char* shader_folder ) = 0;
	};
}
