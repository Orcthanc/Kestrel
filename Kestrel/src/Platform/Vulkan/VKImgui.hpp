#pragma once

#include <kstpch.hpp>
#include "imgui.h"

#include "Platform/Vulkan/VKContext.hpp"
#include "imgui_impl_vulkan.h"

namespace Kestrel {
	struct Imgui_InitInfo {
		Imgui_InitInfo(
				vk::Queue,
				vk::RenderPass,
				uint32_t image_count,
				vk::CommandBuffer,
				vk::Fence = vk::Fence{},
				vk::DescriptorPool = vk::DescriptorPool{} );

		vk::Queue queue;
		vk::RenderPass render_pass;
		uint32_t image_count;
		vk::CommandBuffer cmd_buffer;
		vk::Fence fence;		//Optional
		vk::DescriptorPool desc_pool; //Optional
	};


	struct KST_VK_ImguiWindowData {
		public:
			KST_VK_ImguiWindowData() = default;
			~KST_VK_ImguiWindowData();

			void init( KST_VK_Context& context, Imgui_InitInfo );
			void start();
			void draw( vk::CommandBuffer );
		private:
			bool initialized = false;
			vk::UniqueDescriptorPool desc_pool;
	};
}
