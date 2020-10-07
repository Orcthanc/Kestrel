/*
 * =====================================================================================
 *
 *       Filename:  GLFWWindow.hpp
 *
 *    Description:  GLFW window implementation
 *
 *        Version:  1.0
 *        Created:  09/22/2020 04:46:52 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include <kstpch.hpp>

#include <vulkan/vulkan.hpp>

#include "Core/Window.hpp"

namespace Kestrel {

	struct KSTVKDeviceSurface;

	struct KSTVKSwapchainDetails {
		KSTVKSwapchainDetails() = default;
		KSTVKSwapchainDetails( vk::PhysicalDevice phys, vk::SurfaceKHR surface );

		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;
	};

	struct KST_VK_Surface {
		public:
			KSTVKSwapchainDetails details;
			vk::UniqueSurfaceKHR surface;
	};

	struct KSTVKSwapchain {
		public:
			KSTVKSwapchain() = default;
			KSTVKSwapchain( const KSTVKSwapchain& ) = delete;
			KSTVKSwapchain( KSTVKSwapchain&& ) = default;

			KSTVKSwapchain& operator=( const KSTVKSwapchain& ) = delete;
			KSTVKSwapchain& operator=( KSTVKSwapchain&& ) = default;

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

	class KST_GLFW_VK_Window: public Window {
		public:
			KST_GLFW_VK_Window( WindowSettings settings = {} );
			~KST_GLFW_VK_Window();

			std::pair<unsigned int, unsigned int> getResolution() override;

			void onUpdate() override;

			void setCallback(const EventCallback &e) override;
			void setCursor(const CursorMode &cm) override;

			GLFWwindow* window;
			std::shared_ptr<KSTVKDeviceSurface> device;
			KST_VK_Surface surface;
			KSTVKSwapchain swapchain;
		private:
			WindowSettings w_settings;
	};
}
