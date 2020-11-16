#pragma once

#include "Renderer/Material.hpp"
#include "Renderer/CameraModes.hpp"
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

	using RendererID = void*;

	struct BindingInfo {
		BindingInfo(
				KST_VK_DeviceSurface& device,
				RendererID id,
				size_t dirty_check_id,
				std::vector<std::array<vk::ImageView, 2>>& imgs,
				vk::Extent2D img_size,
				size_t img_bind_index,
				vk::CommandBuffer cmd_buffer,
				vk::Buffer uniform_buffer,
				RenderModeFlags render_mode );

		KST_VK_DeviceSurface& device;
		RendererID id;
		size_t dirty_check_id;
		std::vector<std::array<vk::ImageView, 2>> img_views;
		vk::Extent2D img_size;
		size_t img_bind_index;
		vk::CommandBuffer cmd_buffer;
		vk::Buffer uniform_buffer;
		RenderModeFlags render_mode;
	};

	struct VK_Material_T {
		VK_Material_T() = default;

		VK_Material_T( const VK_Material_T& ) = delete;
		VK_Material_T( VK_Material_T&& ) = default;

		VK_Material_T& operator=( const VK_Material_T& ) = delete;
		VK_Material_T& operator=( VK_Material_T&& ) = default;

		void bind( const BindingInfo& );

		Material id;
		std::vector<vk::UniquePipeline> pipelines;
		vk::UniquePipelineLayout layout;
		vk::UniqueDescriptorSetLayout desc_layout;
		vk::UniqueDescriptorPool desc_pool;
		vk::UniqueRenderPass renderpass;
		std::unordered_map<RendererID, KST_VK_Framebufferset> framebuffers;
		std::unordered_map<RendererID, std::vector<vk::UniqueDescriptorSet>> desc_sets;
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
