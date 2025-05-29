#include "VKContext.hpp"

#include "Core/Application.hpp"
#include "Platform/GLFWWindow.hpp"
#include "Platform/Vulkan/VKMaterial.hpp"
#include "Platform/Vulkan/VKMesh.hpp"
#include "Platform/Vulkan/Base.hpp"
#include "Platform/Vulkan/VKTerrain.hpp"

#include "Scene/Components.hpp"

using namespace Kestrel;

KST_VK_Context* KST_VK_Context::curr_context;

KST_VK_DeviceSurface::~KST_VK_DeviceSurface(){
	//Materials
	VK_Materials::clear();

	// Cameras
	auto view = Application::getInstance()->current_scene->getView<CameraComponent>();
	for( auto& cam: view ){
		view.get<CameraComponent>( cam ).camera->set_renderer( nullptr );
	}

	//Meshes
	VK_MeshRegistry::clear();

	//Terrains
	VK_TerrainRegistry::destroy();
}

const std::vector<const char*> KST_VK_DeviceSurface::dev_exts = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

bool KSTVKQueueFamilies::complete(){
	return graphics.has_value() && present.has_value();
}

void KST_VK_Context::Init( const ContextInformation& c_inf ){
	PROFILE_FUNCTION();

	curr_context = this;

	vk::ApplicationInfo appinfo(
			c_inf.name.c_str(),
			VK_MAKE_VERSION( c_inf.major_version, c_inf.minor_version, c_inf.patch_version ),
			"Kestrel",
			VK_MAKE_VERSION( VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH ),
			VK_API_VERSION_1_4 );


	std::vector<const char*> layers;
#ifndef NDEBUG
	const std::vector<const char*> wanted_layers = {
		"VK_LAYER_KHRONOS_validation",
//		"VK_LAYER_MANGOHUD_overlay",
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
				KST_CORE_WARN( "Could not enable layer {}. (Not present)", l );
		}
	}
#endif //NDEBUG

	//TODO
	uint32_t glfwExtCount;
	const char** glfwExt;

	glfwExt = glfwGetRequiredInstanceExtensions( &glfwExtCount );

	std::vector<const char*> exts( glfwExt, glfwExt + glfwExtCount );

	for( auto& ext: exts ){
		KST_CORE_INFO( "{}", ext );
	}

	vk::InstanceCreateInfo cr_inf( {}, &appinfo, layers, exts );

	instance = vk::createInstanceUnique( cr_inf );

	{
		PROFILE_SCOPE( "Create Surface" );
		//TODO
		for( auto& w: windows ){
			vk::SurfaceKHR temp;
			glfwCreateWindowSurface( static_cast<VkInstance>(*instance), w.window, nullptr, reinterpret_cast<VkSurfaceKHR *>( &temp ));

			w.surface.surface = vk::UniqueSurfaceKHR{ temp, *instance };
		}
	}

	device.create( *this );
}

void KST_VK_Context::onUpdate(){
	PROFILE_FUNCTION();

	for( auto& w: windows ){
		w.onUpdate();
	}
}

void KST_VK_Context::registerWindow( Window &&w ){
	PROFILE_FUNCTION();

	windows.emplace_back( std::move( static_cast<KST_GLFW_VK_Window&&>( w )));
	device.windows = &windows;
}

void KST_VK_Context::setCursorMode( CursorMode mode ){
	PROFILE_FUNCTION();

	for( auto& w: windows ){
		w.setCursor( mode );
	}
}

int KST_VK_Context::getKeyState( int key ){
	return windows[0].getKeyState( key );
}

void KST_VK_DeviceSurface::create( KST_VK_Context& c ){
	PROFILE_FUNCTION();

	windows = &c.windows;
	vk::Instance i{ *c.instance };

	choose_card( {}, i );
	//TODO
	queue_families = find_queue_families( phys_dev,
			*static_cast<KST_VK_Context*>( Application::getInstance()->graphics_context.get() )->windows[0].surface.surface );

	std::unordered_set<uint32_t> families{ queue_families.graphics.value(), queue_families.present.value() };
	const float priorities[] = { 1.0 };

	std::vector<vk::DeviceQueueCreateInfo> dev_q_cr_infs;

	for( auto& f: families ){
		dev_q_cr_infs.push_back({ {}, f, 1, priorities });
	}

	vk::PhysicalDeviceFeatures features;

	//TODO check if available
	features.fillModeNonSolid = VK_TRUE;
	features.tessellationShader = VK_TRUE;

	vk::DeviceCreateInfo dev_cr_inf(
			{},
			dev_q_cr_infs,
			{},
			dev_exts,
			&features
		);

	device = phys_dev.createDeviceUnique( dev_cr_inf );

	swapchains.resize( windows->size() );

	for( size_t i = 0; i < windows->size(); ++i ){
		swapchains[i].Create(
				(*windows)[i].surface.details,
				*(*windows)[i].surface.surface,
				*device );
	}

	VK_Materials::initialize( this );

	vk::CommandPoolCreateInfo pool_cr_inf(
			vk::CommandPoolCreateFlagBits::eTransient,
			queue_families.transfer.value() );

	auto temp = device->createCommandPoolUnique( pool_cr_inf );

	create_render_pass();
	init_meshes( std::move( temp ));
	VK_TerrainRegistry::init( this );
}

void KST_VK_DeviceSurface::create_render_pass(){
	PROFILE_FUNCTION();

	std::array<vk::AttachmentDescription, 2> attachment_descriptions {
		vk::AttachmentDescription(		//image
				{},
				swapchains[0].format.format, //TODO
				vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear,
				vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eTransferSrcOptimal ),

		vk::AttachmentDescription( 		//depthbuffer
				{},
				vk::Format::eD32Sfloat,
				vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear,
				vk::AttachmentStoreOp::eDontCare,
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eDepthStencilAttachmentOptimal )
	};

	std::array<vk::AttachmentReference, 1> attachment_references{
		vk::AttachmentReference( 0, vk::ImageLayout::eColorAttachmentOptimal ),
	};

	vk::AttachmentReference depth_attachment_reference {
		vk::AttachmentReference( 1, vk::ImageLayout::eDepthStencilAttachmentOptimal )
	};

	vk::SubpassDescription subpass_description(
			{},
			vk::PipelineBindPoint::eGraphics,
			{},
			attachment_references,				//Color attachments
			{},									//Resolve attachments
			&depth_attachment_reference,		//Depth attachment
			{} );

	vk::SubpassDependency sub_dependency(
			VK_SUBPASS_EXTERNAL,
			0,
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
			{}, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
			{} );

	vk::RenderPassCreateInfo render_pass_info(
			{},
			2, attachment_descriptions.data(),
			1, &subpass_description,
			1, &sub_dependency );

	renderpass = device->createRenderPassUnique( render_pass_info );
}

void KST_VK_DeviceSurface::init_meshes( vk::UniqueCommandPool&& pool ){
	VK_MeshRegistry::initialize<
		KST_VK_DeviceSurface*,
		vk::Queue,
		vk::UniqueCommandPool&&,
		size_t,
		size_t>(
			this,
			device->getQueue( queue_families.transfer.value(), 0 ),
			std::move( pool ),
			100000 * sizeof( VK_Vertex ),
			600000 * sizeof( uint32_t ));
}

uint32_t KST_VK_DeviceSurface::find_memory_type( uint32_t filter, vk::MemoryPropertyFlags flags ){
	auto memprops = phys_dev.getMemoryProperties();

	for( auto memIndex = 0; auto& m: memprops.memoryTypes ){
		if(( filter & ( 1 << memIndex )) && (( m.propertyFlags & flags ) == flags ))
			return memIndex;
		++memIndex;
	}

	throw std::runtime_error( "No memory available" );
}

KSTVKQueueFamilies KST_VK_DeviceSurface::find_queue_families( vk::PhysicalDevice dev, vk::SurfaceKHR surface ){
	PROFILE_FUNCTION();

	KSTVKQueueFamilies indices;

	auto qfprops = dev.getQueueFamilyProperties();

	for( uint32_t i = 0; i < qfprops.size(); ++i ){
		if( qfprops[i].queueFlags & vk::QueueFlagBits::eGraphics )
			indices.graphics = i;

		if( qfprops[i].queueFlags & vk::QueueFlagBits::eTransfer )
			indices.transfer = i;

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

void KST_VK_DeviceSurface::choose_card( const std::vector<vk::ExtensionProperties>& requiredExtensions, vk::Instance instance ){
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

		for( auto& w: *windows ){
			vk::SurfaceKHR& surface = *w.surface.surface;


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
				(*windows)[i].surface.details = window_swapchain_details[i];
			}
			best_name = std::string( properties.deviceName.begin(), properties.deviceName.end() );
			best_score = score;
			this->phys_dev = phys_dev;
		}
	}

	KST_CORE_INFO( "Chose physical device {} with score of {}", best_name, best_score );
}
