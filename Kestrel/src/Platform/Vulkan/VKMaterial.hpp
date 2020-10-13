#pragma once

#include "Renderer/Material.hpp"
#include <vulkan/vulkan.hpp>

namespace Kestrel {
	struct KST_VK_DeviceSurface;

	struct VK_Material_T {
		Material id;
		vk::UniquePipeline pipeline;
	};

	struct VK_Materials: public Materials {
		public:
			virtual Material loadMaterial( const char* shader_name ) override;

			//TODO wrapper and maybe refcounting
			static VK_Materials& getInstance();

			std::unordered_map<Material, VK_Material_T> materials;
		private:
			KST_VK_DeviceSurface* device;

			friend KST_VK_DeviceSurface;
	};
}
