#pragma once

#include "Renderer/Material.hpp"
#include <vulkan/vulkan.hpp>

namespace Kestrel {
	struct KST_VK_DeviceSurface;

	struct KST_VK_Framebufferset {
		KST_VK_Framebufferset();
		KST_VK_Framebufferset( size_t size );

		operator std::vector<vk::UniqueFramebuffer>&();
		operator const std::vector<vk::UniqueFramebuffer>&() const;

		bool dirty( size_t current_id );

		std::vector<vk::UniqueFramebuffer> buffer;
		//Dirty flag more or less
		size_t lastUpdateID = -1;
	};

	struct VK_Material_T {
		Material id;
		vk::UniquePipeline pipeline;
		vk::UniquePipelineLayout layout;
		vk::UniqueDescriptorSetLayout desc_layout;
		vk::UniqueDescriptorPool desc_pool;
		vk::UniqueRenderPass renderpass;
		std::unordered_map<void*, KST_VK_Framebufferset> framebuffers;
		std::unordered_map<void*, std::vector<vk::UniqueDescriptorSet>> desc_sets;
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
