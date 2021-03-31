#pragma once

#include "Resource/Resource.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/CameraModes.hpp"
#include <vulkan/vulkan.hpp>

#include <filesystem>
#include <map>

namespace Kestrel {
	struct KST_VK_DeviceSurface;

	using RendererID = void*;

	struct BindingInfo {
		BindingInfo(
				KST_VK_DeviceSurface& device,
				RendererID id,
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

		uint32_t id;
		std::vector<vk::UniquePipeline> pipelines;
		vk::UniquePipelineLayout layout;
		vk::UniqueDescriptorSetLayout desc_layout;
		vk::UniqueDescriptorPool desc_pool;
		std::unordered_map<RendererID, std::vector<vk::UniqueDescriptorSet>> desc_sets;
	};

	struct SharedMaterialResources {
		KST_VK_DeviceSurface* device;
		std::vector<vk::UniqueFramebuffer> framebuffers;
	};

	using VK_Materials = ResourceRegistry<VK_Material_T, Material, SharedMaterialResources, std::filesystem::path>;

	template<>template<>
	void VK_Materials::initialize( KST_VK_DeviceSurface* device );
}
