#pragma once

#include "Renderer/Material.hpp"
#include <vulkan/vulkan.hpp>

namespace Kestrel {
	struct KST_VK_DeviceSurface;

	struct VK_Material_T {
		Material id;
		vk::UniquePipeline pipeline;
		vk::UniquePipelineLayout layout;
		vk::UniqueRenderPass renderpass;
		std::vector<vk::UniqueFramebuffer> framebuffers;
	};

	struct VK_Materials: public Materials {
		public:
			virtual Material loadMaterial( const char* shader_name ) override;

			//TODO wrapper and maybe refcounting
			static VK_Materials& getInstance();

			VK_Material_T& operator[]( Material mat );
			const VK_Material_T& operator[]( Material mat ) const;

			std::vector<vk::UniqueFramebuffer> framebuffers;

		private:
			std::unordered_map<Material, VK_Material_T> materials;
			KST_VK_DeviceSurface* device;

			friend KST_VK_DeviceSurface;
	};
}
