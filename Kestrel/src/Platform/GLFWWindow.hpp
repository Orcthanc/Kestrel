#pragma once

#include <kstpch.hpp>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Core/Window.hpp"

namespace Kestrel {

	struct KST_VK_DeviceSurface;

	struct KSTVKSwapchainDetails {
		KSTVKSwapchainDetails() = default;
		KSTVKSwapchainDetails( vk::PhysicalDevice phys, vk::SurfaceKHR surface );

		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;
	};

	struct KST_VK_Surface;

	struct KST_VK_Swapchain {
		public:
			KST_VK_Swapchain() = default;
			KST_VK_Swapchain( const KST_VK_Swapchain& ) = delete;
			KST_VK_Swapchain( KST_VK_Swapchain&& ) = default;

			KST_VK_Swapchain& operator=( const KST_VK_Swapchain& ) = delete;
			KST_VK_Swapchain& operator=( KST_VK_Swapchain&& ) = default;

			void create( KST_VK_Surface& surface, KST_VK_DeviceSurface& device );
			void Create( const KSTVKSwapchainDetails&, vk::SurfaceKHR surface, vk::Device device );

			vk::UniqueSwapchainKHR swapchain;
			std::vector<vk::Image> images;
			vk::SurfaceFormatKHR format;
			vk::Extent2D size;
			//std::vector<vk::UniqueImageView> views;
			KSTVKSwapchainDetails details;

		private:
			vk::SurfaceFormatKHR find_format( const KSTVKSwapchainDetails& capabilities );
			vk::PresentModeKHR find_mode( const KSTVKSwapchainDetails& capabilities );
			vk::Extent2D find_extent( const KSTVKSwapchainDetails& capabilities );
	};

	struct KST_VK_Surface {
		vk::UniqueSurfaceKHR surface;
		KSTVKSwapchainDetails details;
	};

	class KST_GLFW_VK_Window: public Window {
		public:
			KST_GLFW_VK_Window( WindowSettings settings = {} );
			~KST_GLFW_VK_Window();

			KST_GLFW_VK_Window( const KST_GLFW_VK_Window& ) = delete;
			KST_GLFW_VK_Window( KST_GLFW_VK_Window&& ) noexcept;

			KST_GLFW_VK_Window& operator=( const KST_GLFW_VK_Window& ) = delete;
			KST_GLFW_VK_Window& operator=( KST_GLFW_VK_Window&& ) noexcept;

			std::pair<unsigned int, unsigned int> getResolution() override;

			void onUpdate() override;

			void setCallback(const EventCallback &e) override;
			void setCursor(const CursorMode &cm) override;

			GLFWwindow* window;
			KST_VK_Surface surface;
			WindowSettings w_settings;
		private:
			bool destroy = true;
	};
}
