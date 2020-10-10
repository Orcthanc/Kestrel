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
		//TODO

		bool complete();
	};

	struct KSTVKContext;

	struct KSTVKDeviceSurface {
		public:
			KSTVKDeviceSurface() = default;

			void create( KSTVKContext& instance );

			vk::PhysicalDevice phys_dev;
			KSTVKQueueFamilies queue_families;
			vk::UniqueDevice device;
			std::vector<KSTVKSwapchain> swapchains;
			std::vector<KST_GLFW_VK_Window>* windows;

		private:
			void choose_card( const std::vector<vk::ExtensionProperties>& requiredExtensions, vk::Instance instance );
			KSTVKQueueFamilies find_queue_families( vk::PhysicalDevice dev, vk::SurfaceKHR surface );

			const static std::vector<const char*> dev_exts;
	};

	struct KSTVKContext: public Context {
		public:
			KSTVKContext() = default;

			virtual void Init( const ContextInformation& ) override;
			virtual void onUpdate() override;
			virtual void registerWindow( Window&& w ) override;
			vk::UniqueInstance instance;
			std::vector<KST_GLFW_VK_Window> windows;
		private:
			KSTVKDeviceSurface device;
	};
}
