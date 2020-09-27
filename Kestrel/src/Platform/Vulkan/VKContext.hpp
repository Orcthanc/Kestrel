#pragma once

#include "Renderer/Context.hpp"

#include "vulkan/vulkan.hpp"

namespace Kestrel {
	struct VKContext: public Context {
		public:
			VKContext() = default;

			virtual void Init( const ContextInformation& ) override;
		private:
			vk::UniqueInstance instance;
	};
}
