#include "VKContext.hpp"
#include "Core/Application.hpp"
#include <GLFW/glfw3.h>
#include "Platform/GLFWWindow.hpp"

using namespace Kestrel;


bool KSTVKQueueFamilies::complete(){
	return graphics.has_value() && present.has_value();
}

void KSTVKContext::Init( const ContextInformation& c_inf ){
	PROFILE_FUNCTION();

	vk::ApplicationInfo appinfo(
			c_inf.name.c_str(),
			VK_MAKE_VERSION( c_inf.major_version, c_inf.minor_version, c_inf.patch_version ),
			"Kestrel",
			VK_MAKE_VERSION( VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH ),
			VK_API_VERSION_1_2 );


	std::vector<const char*> layers;
#ifndef NDEBUG
	const std::vector<const char*> wanted_layers = {
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_MANGOHUD_overlay",
//		"VK_LAYER_LUNARG_api_dump",
	};

	{

		auto d_layers = vk::enumerateInstanceLayerProperties();

		for( auto l: wanted_layers ){
			bool found = false;
			for( auto& l2: d_layers ){
				if( !strcmp( l, l2.layerName )){
					found = true;
					KST_CORE_INFO( "Enabled vulkan layer {}", l );
					layers.push_back( l );
					found = true;
					break;
				}
			}
			if( !found )
				KST_CORE_INFO( "Could not enable layer {}. (Not present)", l );
		}
	}
#endif //NDEBUG

	//TODO
	uint32_t glfwExtCount;
	const char** glfwExt;

	glfwExt = glfwGetRequiredInstanceExtensions( &glfwExtCount );

	std::vector<const char*> exts( glfwExt, glfwExt + glfwExtCount );

	vk::InstanceCreateInfo cr_inf( {}, &appinfo, layers, exts );

	instance = vk::createInstanceUnique( cr_inf );

	{
		PROFILE_SCOPE( "Create Surface" );
		//TODO
		for( auto& w: Application::getInstance()->window ){
			vk::SurfaceKHR temp;
			glfwCreateWindowSurface(static_cast<VkInstance>(*instance), static_cast<KST_GLFW_VK_Window*>( w.get() )->window, nullptr, reinterpret_cast<VkSurfaceKHR *>(&temp));

			static_cast<KST_GLFW_VK_Window*>( w.get())->surface.surface = vk::UniqueSurfaceKHR{ temp, *instance };
		}
	}

	device.create( *instance );

	for( auto& w: Application::getInstance()->window ){
		static_cast<KST_GLFW_VK_Window*>( w.get())->swapchain.Create(
				static_cast<KST_GLFW_VK_Window*>( w.get())->surface.details,
				*static_cast<KST_GLFW_VK_Window*>( w.get())->surface.surface,
				*device.device );
	}
}


void KSTVKDeviceSurface::create( vk::Instance i ){
	PROFILE_FUNCTION();

	choose_card( {}, i );
	queue_families = find_queue_families( phys_dev,
			*static_cast<KST_GLFW_VK_Window*>( Application::getInstance()->window[0].get())->surface.surface );

	std::unordered_set<uint32_t> families{ queue_families.graphics.value(), queue_families.present.value() };
	const float priorities[] = { 1.0 };

	std::vector<vk::DeviceQueueCreateInfo> dev_q_cr_infs;

	for( auto& f: families ){
		dev_q_cr_infs.push_back({ {}, f, 1, priorities });
	}

	vk::PhysicalDeviceFeatures features;

	vk::DeviceCreateInfo dev_cr_inf(
			{},
			dev_q_cr_infs,
			{},
			dev_exts,
			&features
		);

	device = phys_dev.createDeviceUnique( dev_cr_inf );
}

KSTVKQueueFamilies KSTVKDeviceSurface::find_queue_families( vk::PhysicalDevice dev, vk::SurfaceKHR surface ){
	PROFILE_FUNCTION();

	KSTVKQueueFamilies indices;

	auto qfprops = dev.getQueueFamilyProperties();

	for( uint32_t i = 0; i < qfprops.size(); ++i ){
		if( qfprops[i].queueFlags & vk::QueueFlagBits::eGraphics )
			indices.graphics = i;

		if( dev.getSurfaceSupportKHR( i, surface ))
			indices.present = i;

		if( indices.complete())
			break;
	}

	return indices;
}

static std::unordered_set<std::string> get_missing_dev_extensions( vk::PhysicalDevice dev, const std::vector<const char*>& extensions ){
	PROFILE_FUNCTION();

	auto available_exts = dev.enumerateDeviceExtensionProperties();

	std::unordered_set<std::string> req_exts{ extensions.begin(), extensions.end() };

	for( auto& ext: available_exts ){
		req_exts.erase( ext.extensionName );
	}

	return req_exts;
}

void KSTVKDeviceSurface::choose_card( const std::vector<vk::ExtensionProperties>& requiredExtensions, vk::Instance instance ){
	PROFILE_FUNCTION();
	auto physical_devs{ instance.enumeratePhysicalDevices() };

	size_t best_score = 0;
	std::string best_name;

	for( auto& phys_dev: physical_devs ){
		size_t score = 0;
		std::vector<vk::ExtensionProperties> exts = phys_dev.enumerateDeviceExtensionProperties();

		bool supported = true;
		for( auto& ext: requiredExtensions ){
			if( std::find( exts.begin(), exts.end(), ext ) == exts.end()){
				supported = false;
			}
		}

		if( !supported )
			continue;

		if( !get_missing_dev_extensions( phys_dev, dev_exts ).empty() )
			continue;

		bool suitable = true;

		std::vector<KSTVKSwapchainDetails> window_swapchain_details;

		for( auto& w: Application::getInstance()->window ){
			vk::SurfaceKHR surface = *static_cast<KST_GLFW_VK_Window*>( w.get())->surface.surface;


			auto qfindices = find_queue_families( phys_dev, surface );
			if( !qfindices.complete() ){
				suitable = false;
				break;
			}

			KSTVKSwapchainDetails swapchain_details( phys_dev, surface );
			if( swapchain_details.formats.empty() || swapchain_details.present_modes.empty() ){
				suitable = false;
				break;
			}
			window_swapchain_details.push_back( swapchain_details );
		}

		if( !suitable ){
			continue;
		}


		auto properties = phys_dev.getProperties();
		if( properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu )
			score += 50000;
		else if( properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu )
			score += 5000;

		score += properties.limits.maxImageDimension2D;

		KST_CORE_INFO( "Found suitable physical device {} with score {}.", properties.deviceName, score );

		if( score > best_score ){
			for( size_t i = 0; i < window_swapchain_details.size(); ++i ){
				static_cast<KST_GLFW_VK_Window*>( Application::getInstance()->window[i].get())->surface.details = window_swapchain_details[i];
			}
			best_name = std::string( properties.deviceName.begin(), properties.deviceName.end() );
			best_score = score;
			this->phys_dev = phys_dev;
		}
	}

	KST_CORE_INFO( "Chose physical device {} with score of {}", best_name, best_score );
}
