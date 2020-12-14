#pragma once

namespace Kestrel {
	enum class Platform {
		None,
		Windows,
		Linux,
	};

	enum class RenderPath {
		None,
		Vulkan,
	};

	extern Platform platform;
	extern RenderPath render_path;
};
