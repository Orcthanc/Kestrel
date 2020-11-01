#pragma once

#include <kstpch.hpp>
#include "Core/Window.hpp"
#include "Scene/Entity.hpp"

namespace Kestrel {
	struct ContextInformation {
		const std::string name;
		uint16_t major_version, minor_version, patch_version;
	};

	struct Context {
		virtual ~Context(){
			KST_CORE_INFO( "Cleaning up graphics context" );
		};

		virtual void Init( const ContextInformation& ) = 0;
		virtual void onUpdate() = 0;
		virtual void registerWindow( Window&& w ) = 0;
	};
}
