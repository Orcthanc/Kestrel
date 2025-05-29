#pragma once

#include <kstpch.hpp>
#include <optional>

#include "Renderer/Context.hpp"

#include "vulkan/vulkan.hpp"

#include "Platform/GLFWWindow.hpp"

namespace Kestrel {

	struct KSTVKQueueFamilies {
		std::optional<uint32_t> graphics;
		std::optional<uint32_t> present;
		std::optional<uint32_t> transfer;
		//TODO

		bool complete();
	};

	struct KST_VK_Context;

	struct KST_VK_DeviceSurface {
		public:
			KST_VK_DeviceSurface() = default;
			~KST_VK_DeviceSurface();

			void create( KST_VK_Context& instance );
			uint32_t find_memory_type( uint32_t filter, vk::MemoryPropertyFlags flags );

			vk::PhysicalDevice phys_dev;
			KSTVKQueueFamilies queue_families;
			vk::UniqueDevice device;
			std::vector<KST_VK_Swapchain> swapchains;
			std::vector<KST_GLFW_VK_Window>* windows = nullptr; // Managed by context

			vk::UniqueRenderPass renderpass;

		private:
			void choose_card( const std::vector<vk::ExtensionProperties>& requiredExtensions, vk::Instance instance );
			void init_meshes( vk::UniqueCommandPool&& transfer_pool );
			void create_render_pass();
			KSTVKQueueFamilies find_queue_families( vk::PhysicalDevice dev, vk::SurfaceKHR surface );

			const static std::vector<const char*> dev_exts;

	};

	struct KST_VK_Context: public Context {
		public:
			KST_VK_Context() = default;

			virtual void Init( const ContextInformation& ) override;
			virtual void onUpdate() override;
			virtual void registerWindow( Window&& w ) override;
			virtual void setCursorMode( CursorMode mode ) override;
			virtual int getKeyState( int key ) override;

			vk::UniqueInstance instance;
			std::vector<KST_GLFW_VK_Window> windows;
			KST_VK_DeviceSurface device;

			static KST_VK_Context& get(){
				return *curr_context;
			}
		private:
			static KST_VK_Context* curr_context;
	};
}
