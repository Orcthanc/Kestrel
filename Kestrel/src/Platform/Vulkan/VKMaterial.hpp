#pragma once

#include "Renderer/Material.hpp"
#include <vulkan/vulkan.hpp>

namespace Kestrel {
	struct VK_Material_T {
		Material id;
		vk::UniquePipeline pipeline;
	};

	struct Materials {
		//TODO wrapper and maybe refcounting
		static std::unordered_map<Material, VK_Material_T>& getInstance(){
			static std::unordered_map<Material, VK_Material_T> materials;
			return materials;
		}
	};
}
