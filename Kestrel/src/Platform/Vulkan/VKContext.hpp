#pragma once

#include <kstpch.hpp>
#include <optional>

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

	struct KSTVKSwapchain {
		public:
			KSTVKSwapchain() = default;

			void Create( const KSTVKSwapchainDetails&, vk::SurfaceKHR surface, vk::Device device );

			vk::UniqueSwapchainKHR swapchain;
			std::vector<vk::Image> images;
			vk::SurfaceFormatKHR format;
			vk::Extent2D size;
			std::vector<vk::UniqueImageView> views;

		private:
			vk::SurfaceFormatKHR find_format( const KSTVKSwapchainDetails& capabilities );
			vk::PresentModeKHR find_mode( const KSTVKSwapchainDetails& capabilities );
			vk::Extent2D find_extent( const KSTVKSwapchainDetails& capabilities );
	};

	struct KSTVKDeviceSurface {
		public:
			KSTVKDeviceSurface( vk::Instance instance, vk::SurfaceKHR surface );

			vk::PhysicalDevice phys_dev;
			KSTVKQueueFamilies queue_families;
			vk::UniqueDevice device;
			KSTVKSwapchainDetails swapchain_support;

		private:
			void choose_card( const std::vector<vk::ExtensionProperties>& requiredExtensions, vk::Instance instance, vk::SurfaceKHR surface );
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
			vk::UniqueSurfaceKHR surface;
			KSTVKSwapchain swapchain;
			std::optional<KSTVKDeviceSurface> device;
	};
}
