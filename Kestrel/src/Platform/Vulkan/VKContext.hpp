#pragma once

#include <kstpch.hpp>
#include <optional>

#include "Renderer/Context.hpp"

#include "vulkan/vulkan.hpp"

namespace Kestrel {

	struct KSTVKQueueFamilies {
		std::optional<uint32_t> graphics;
		std::optional<uint32_t> present;
		//TODO

		bool complete();
	};

	struct KSTVKDeviceSurface {
		public:
			KSTVKDeviceSurface() = default;

			void create( vk::Instance instance );

			vk::PhysicalDevice phys_dev;
			KSTVKQueueFamilies queue_families;
			vk::UniqueDevice device;

		private:
			void choose_card( const std::vector<vk::ExtensionProperties>& requiredExtensions, vk::Instance instance );
			KSTVKQueueFamilies find_queue_families( vk::PhysicalDevice dev, vk::SurfaceKHR surface );

			const std::vector<const char*> dev_exts = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			};
	};

	struct KSTVKContext: public Context {
		public:
			KSTVKContext() = default;

			virtual void Init( const ContextInformation& ) override;
		private:
			vk::UniqueInstance instance;
			KSTVKDeviceSurface device;
	};
}
