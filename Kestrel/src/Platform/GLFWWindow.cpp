#include "GLFWWindow.hpp"
#include "Core/Application.hpp"
#include "Platform/Vulkan/VKContext.hpp"

using namespace Kestrel;

static int glfwInitialized = 0;

KST_GLFW_VK_Window::KST_GLFW_VK_Window( WindowSettings s ): w_settings{ std::move( s )}{
	PROFILE_FUNCTION();

	if( !glfwInitialized++ ){
		if( !glfwInit() ){
			KST_CORE_CRITICAL( "Could not create glfw context" );
			throw std::runtime_error( "Could not create glfw context" );
		}

		glfwSetErrorCallback( []( int error, const char* description ){ KST_CORE_ERROR( "GLFW Error: {} ({})", error, description ); });
	}

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	window = glfwCreateWindow( w_settings.width, w_settings.height, w_settings.name.c_str(), NULL, NULL );

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
	if( destroy ){
		glfwDestroyWindow( window );
		if( --glfwInitialized == 0 ){
			glfwTerminate();
		}
	}
}

KST_GLFW_VK_Window::KST_GLFW_VK_Window( KST_GLFW_VK_Window&& w ){
	window = std::move( w.window );
	surface = std::move( w.surface );
	w_settings = std::move( w.w_settings );

	glfwSetWindowUserPointer( window, &w_settings );

	w.window = nullptr;
	w.destroy = false;
}

KST_GLFW_VK_Window& KST_GLFW_VK_Window::operator=( KST_GLFW_VK_Window&& w ){
	if( &w == this ){
		return *this;
	}

	window = std::move( w.window );
	surface = std::move( w.surface );
	w_settings = std::move( w.w_settings );

	glfwSetWindowUserPointer( window, &w_settings );

	w.window = nullptr;
	w.destroy = false;
	return *this;
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

vk::SurfaceFormatKHR KST_VK_Swapchain::find_format( const KSTVKSwapchainDetails& capabilities ){
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

vk::PresentModeKHR KST_VK_Swapchain::find_mode( const KSTVKSwapchainDetails& capabilities ){
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

vk::Extent2D KST_VK_Swapchain::find_extent( const KSTVKSwapchainDetails& capabilities ){
	PROFILE_FUNCTION();

	if( capabilities.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() ){
		return capabilities.capabilities.currentExtent;
	} else {
		//TODO
//		auto Wsize = Application::getInstance()->window[0]->getResolution();
//		auto size = vk::Extent2D{ Wsize.first, Wsize.second };
		auto size = vk::Extent2D{ 1920, 1080 };

		size = std::max( capabilities.capabilities.minImageExtent,
				std::min( capabilities.capabilities.maxImageExtent, size ));
		return size;
	}
}

void KST_VK_Swapchain::create( KST_VK_Surface &surface, KST_VK_DeviceSurface& device ){
	PROFILE_FUNCTION();

	surface.details = KSTVKSwapchainDetails( device.phys_dev, *surface.surface );

	Create( surface.details, *surface.surface, *device.device );
}

void KST_VK_Swapchain::Create( const KSTVKSwapchainDetails& capabilities, vk::SurfaceKHR surface, vk::Device device ){
	PROFILE_FUNCTION();

	format = find_format( capabilities );
	auto present_mode = find_mode( capabilities );
	size = find_extent( capabilities );

	//Image count
	uint32_t count = capabilities.capabilities.minImageCount + 1;
	if( capabilities.capabilities.maxImageCount > 0 && capabilities.capabilities.maxImageCount < count ){
		count = capabilities.capabilities.maxImageCount;
	}

	KST_CORE_INFO( "Resizing swapchain to size {} x {}", size.width, size.height );

	//Create swapchain
	vk::SwapchainCreateInfoKHR cr_inf(
			{},
			surface,
			count,
			format.format,
			format.colorSpace,
			size,
			1,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive,
			{},
			capabilities.capabilities.currentTransform,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			present_mode,
			true,
			{} );

	swapchain = device.createSwapchainKHRUnique( cr_inf );

	images = device.getSwapchainImagesKHR( *swapchain );
}
