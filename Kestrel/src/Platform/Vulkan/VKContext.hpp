#pragma once

#include "Renderer/Context.hpp"

#include "vulkan/vulkan.hpp"

namespace Kestrel {

	struct KSTVKSwapchainDetails {
		KSTVKSwapchainDetails() = default;
		KSTVKSwapchainDetails( vk::PhysicalDevice phys, vk::SurfaceKHR surface );

		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;
	};

	struct KSTVKQueueFamilies {
		std::optional<uint32_t> graphics;
		std::optional<uint32_t> present;
		//TODO
		
		bool complete();
	};

	struct KSTVKDevice {
		public:
			KSTVKDevice( vk::Instance instance, vk::SurfaceKHR surface );

		private:
			void choose_card( const std::vector<vk::ExtensionProperties>& requiredExtensions, vk::Instance instance, vk::SurfaceKHR surface );
			KSTVKQueueFamilies find_queue_families( vk::PhysicalDevice dev, vk::SurfaceKHR surface );

			vk::PhysicalDevice phys_dev;
			KSTVKQueueFamilies queue_families;
			vk::UniqueDevice device;
			KSTVKSwapchainDetails swapchain_support;

			std::vector<const char*> dev_exts = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			};
	};

	struct KSTVKContext: public Context {
		public:
			KSTVKContext() = default;

			virtual void Init( const ContextInformation& ) override;
		private:
			vk::UniqueInstance instance;
			vk::UniqueSurfaceKHR surface;
			std::optional<KSTVKDevice> device;
	};
}
