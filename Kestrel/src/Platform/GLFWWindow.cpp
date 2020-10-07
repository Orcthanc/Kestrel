/*
 * =====================================================================================
 *
 *       Filename:  KST_GLFWWindow.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/26/2020 02:27:14 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include "GLFWWindow.hpp"
#include "Core/Application.hpp"

using namespace Kestrel;

static int glfwInitialized = 0;

KST_GLFW_VK_Window::KST_GLFW_VK_Window( WindowSettings s ): w_settings{ s }{

	if( !glfwInitialized++ ){
		if( !glfwInit() ){
			KST_CORE_CRITICAL( "Could not create glfw context" );
			throw std::runtime_error( "Could not create glfw context" );
		}

		glfwSetErrorCallback( []( int error, const char* description ){ KST_CORE_ERROR( "GLFW Error: {} ({})" ); });
	}

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	window = glfwCreateWindow( s.width, s.height, s.name.c_str(), NULL, NULL );

	if( !window ){
		KST_CORE_CRITICAL( "Could not create glfw window" );
		throw std::runtime_error( "Could not create glfw window" );
	}

	glfwSetWindowUserPointer( window, &w_settings );

	glfwSetWindowSizeCallback( window, []( GLFWwindow* window, int width, int height ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			s->width = width;
			s->height = height;
		});

	glfwSetWindowCloseCallback( window, []( GLFWwindow* window ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			WindowCloseEvent e;
			s->callback( e );
		});

	glfwSetKeyCallback( window, []( GLFWwindow* window, int key, int scancode, int action, int mods ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			switch( action ){
				case GLFW_PRESS:
				{
					KeyPushEvent e{ scancode };
					s->callback( e );
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleaseEvent e{ scancode };
					s->callback( e );
					break;
				}
				case GLFW_REPEAT:
				{
					KST_CORE_WARN( "GLFW_REPEAT unimplemented" );
					break;
				}
			}
		});

	glfwSetCursorPosCallback( window, []( GLFWwindow* window, double x, double y ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			MouseMovedEvent e( x, y );
			s->callback( e );
		});

	glfwSetScrollCallback( window, []( GLFWwindow* window, double x, double y ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			MouseScrollEvent e( x, y );
			s->callback( e );
		});

	glfwSetMouseButtonCallback( window, []( GLFWwindow* window, int button, int action, int mods ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			switch( action ){
				case GLFW_PRESS:
				{
					MousePressedEvent e( button );
					s->callback( e );
					break;
				}
				case GLFW_RELEASE:
				{
					MouseReleasedEvent e( button );
					s->callback( e );
					break;
				}
			}
		});
}

KST_GLFW_VK_Window::~KST_GLFW_VK_Window(){
	glfwDestroyWindow( window );
	KST_INFO( "Device ref count {}", device.use_count() );
	if( --glfwInitialized == 0 ){
		glfwTerminate();
	}
}

std::pair<unsigned int, unsigned int> KST_GLFW_VK_Window::getResolution(){
	return { w_settings.width, w_settings.height };
}

void KST_GLFW_VK_Window::onUpdate(){
	glfwPollEvents();
}

void KST_GLFW_VK_Window::setCallback( const EventCallback& e ){
	w_settings.callback = e;
}

void KST_GLFW_VK_Window::setCursor( const CursorMode& cm ){
	switch( cm ){
		case CursorMode::Normal:
		{
			KST_INFO( "Normal" );
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
			break;
		}
		case CursorMode::Hidden:
		{
			KST_INFO( "Hidden" );
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
			break;
		}
		case CursorMode::Disabled:
		{
			KST_INFO( "Disabled" );
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
			break;
		}
		default:
		{
			KST_CORE_ERROR( "Invalid cursor mode {}", static_cast<std::underlying_type_t<CursorMode>>( cm ));
			break;
		}
	}
}

KSTVKSwapchainDetails::KSTVKSwapchainDetails( vk::PhysicalDevice phys, vk::SurfaceKHR surface ){
	PROFILE_FUNCTION();
	capabilities = phys.getSurfaceCapabilitiesKHR( surface );
	formats = phys.getSurfaceFormatsKHR( surface );
	present_modes = phys.getSurfacePresentModesKHR( surface );
}

vk::SurfaceFormatKHR KSTVKSwapchain::find_format( const KSTVKSwapchainDetails& capabilities ){
	PROFILE_FUNCTION();

	for( const auto& format: capabilities.formats ){
		if( format.format == vk::Format::eB8G8R8A8Srgb
				&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear ){
			return format;
		}
	}
	KST_CORE_WARN( "Falling back to format {}/{}",
			vk::to_string( capabilities.formats[0].format ),
			vk::to_string( capabilities.formats[0].colorSpace ));
	return capabilities.formats[0];
}

vk::PresentModeKHR KSTVKSwapchain::find_mode( const KSTVKSwapchainDetails& capabilities ){
	PROFILE_FUNCTION();

	for( const auto& mode: capabilities.present_modes ){
		//TODO vsync on/off
		if( mode == vk::PresentModeKHR::eMailbox ){
			return mode;
		}
	}
	KST_CORE_WARN( "Falling back to Fifo present mode" );
	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D KSTVKSwapchain::find_extent( const KSTVKSwapchainDetails& capabilities ){
	PROFILE_FUNCTION();

	if( capabilities.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() ){
		return capabilities.capabilities.currentExtent;
	} else {
		//TODO
		auto Wsize = Application::getInstance()->window[0]->getResolution();
		auto size = vk::Extent2D{ Wsize.first, Wsize.second };

		size = std::max( capabilities.capabilities.minImageExtent,
				std::min( capabilities.capabilities.maxImageExtent, size ));
		return size;
	}
}

void KSTVKSwapchain::Create( const KSTVKSwapchainDetails& capabilities, vk::SurfaceKHR surface, vk::Device device ){
	PROFILE_FUNCTION();

	format = find_format( capabilities );
	auto present_mode = find_mode( capabilities );
	size = find_extent( capabilities );

	//Image count
	uint32_t count = capabilities.capabilities.minImageCount + 1;
	if( capabilities.capabilities.maxImageCount > 0 && capabilities.capabilities.maxImageCount < count ){
		count = capabilities.capabilities.maxImageCount;
	}

	//Create swapchain
	vk::SwapchainCreateInfoKHR cr_inf(
			{},
			surface,
			count,
			format.format,
			format.colorSpace,
			size,
			1,
			vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive,
			{},
			capabilities.capabilities.currentTransform,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			present_mode,
			true,
			{} );

	swapchain = device.createSwapchainKHRUnique( cr_inf );
}