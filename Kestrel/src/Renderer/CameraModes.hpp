#pragma once

#include <kstpch.hpp>

namespace Kestrel {
	enum class RenderModeFlags {
		eNone = 0x0,
		eInverse = 0x1,
		eLogarithmic = 0x2,
		eIntegerDepth = 0x4,
		eWireframe = 0x8,
		eAllFlags = 0xF,
	};

	std::string to_string( RenderModeFlags );
};
