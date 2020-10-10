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
#pragma once

#include <kstpch.hpp>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

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
			KSTVKSwapchainDetails details;

		private:
			vk::SurfaceFormatKHR find_format( const KSTVKSwapchainDetails& capabilities );
			vk::PresentModeKHR find_mode( const KSTVKSwapchainDetails& capabilities );
			vk::Extent2D find_extent( const KSTVKSwapchainDetails& capabilities );
	};

	struct KSTVKSurface {
		vk::UniqueSurfaceKHR surface;
		KSTVKSwapchainDetails details;
	};

	class KST_GLFW_VK_Window: public Window {
		public:
			KST_GLFW_VK_Window( WindowSettings settings = {} );
			~KST_GLFW_VK_Window();

			KST_GLFW_VK_Window( const KST_GLFW_VK_Window& ) = delete;
			KST_GLFW_VK_Window( KST_GLFW_VK_Window&& );

			KST_GLFW_VK_Window& operator=( const KST_GLFW_VK_Window& ) = delete;
			KST_GLFW_VK_Window& operator=( KST_GLFW_VK_Window&& );

			std::pair<unsigned int, unsigned int> getResolution() override;

			void onUpdate() override;

			void setCallback(const EventCallback &e) override;
			void setCursor(const CursorMode &cm) override;

			GLFWwindow* window;
			KSTVKSurface surface;
			WindowSettings w_settings;
		private:
			bool destroy = true;
	};
}
