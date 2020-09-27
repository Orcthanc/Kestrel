#pragma once

#include <kstpch.hpp>

namespace Kestrel {
	struct ContextInformation {
		const std::string name;
		uint16_t major_version, minor_version, patch_version;
	};

	struct Context {
		virtual ~Context() = default;

		virtual void Init( const ContextInformation& ) = 0;
	};
}
