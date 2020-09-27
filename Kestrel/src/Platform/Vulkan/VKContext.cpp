#include "VKContext.hpp"
#include <GLFW/glfw3.h>

using namespace Kestrel;

void VKContext::Init( const ContextInformation& c_inf ){
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
		"VK_LAYER_LUNARG_api_dump",
	};

	{

		auto d_layers = vk::enumerateInstanceLayerProperties();

		for( auto l: wanted_layers ){
			bool found = false;
			for( auto& l2: d_layers ){
				if( !strcmp( l, l2.layerName )){
					found = true;
					KST_INFO( "Enabled vulkan layer {}", l );
					layers.push_back( l );
					break;
				}
			}
			KST_INFO( "Could not enable layer {}. (Not present)", l );
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
}
