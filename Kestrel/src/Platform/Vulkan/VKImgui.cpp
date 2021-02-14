#include "Platform/Vulkan/VKImgui.hpp"

#define GLFW_INCLUDE_VULKAN
#include "imgui_impl_glfw.h"

using namespace Kestrel;

Imgui_InitInfo::Imgui_InitInfo( vk::Queue queue, vk::RenderPass render_pass, uint32_t image_count, vk::CommandBuffer cmd_buffer, vk::Fence fence, vk::DescriptorPool pool ):
	queue( queue ), render_pass( render_pass ), image_count( image_count ), cmd_buffer( cmd_buffer ), fence( fence ), desc_pool( pool ){}

void KST_VK_ImguiWindowData::init( KST_VK_Context& context, Imgui_InitInfo init_info ) {
	PROFILE_FUNCTION();

	if( initialized ){
		ImGui_ImplVulkan_Shutdown();
	} else {
		ImGui::CreateContext();

		ImGui_ImplGlfw_InitForVulkan( context.windows[0].window, true ); //TODO set to false
	}

	ImGui_ImplVulkan_InitInfo imgui_vulkan_inf;

	memset( &imgui_vulkan_inf, 0, sizeof( imgui_vulkan_inf ));

	if( !init_info.desc_pool ){
		std::array<vk::DescriptorPoolSize, 11> pool_sizes {
			vk::DescriptorPoolSize( vk::DescriptorType::eSampler, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eCombinedImageSampler, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eSampledImage, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eStorageImage, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eUniformTexelBuffer, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eStorageTexelBuffer, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eUniformBuffer, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eStorageBuffer, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eUniformBufferDynamic, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eStorageBufferDynamic, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eInputAttachment, 1000 )
		};

		vk::DescriptorPoolCreateInfo desc_inf(
				vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
				1000,
				pool_sizes );

		desc_pool = context.device.device->createDescriptorPoolUnique( desc_inf );
		imgui_vulkan_inf.DescriptorPool = *desc_pool;
	} else {
		imgui_vulkan_inf.DescriptorPool = init_info.desc_pool;
	}

	imgui_vulkan_inf.Device = *context.device.device;
	imgui_vulkan_inf.ImageCount = init_info.image_count;
	imgui_vulkan_inf.MinImageCount = init_info.image_count;
	imgui_vulkan_inf.Instance = *context.instance;
	imgui_vulkan_inf.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	imgui_vulkan_inf.PhysicalDevice = context.device.phys_dev;
	imgui_vulkan_inf.PipelineCache = vk::PipelineCache{};
	imgui_vulkan_inf.Queue = init_info.queue;

	ImGui_ImplVulkan_Init( &imgui_vulkan_inf, init_info.render_pass );

	vk::CommandBufferBeginInfo beg_inf(
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
			{} );

	init_info.cmd_buffer.begin( beg_inf );

	ImGui_ImplVulkan_CreateFontsTexture( init_info.cmd_buffer );

	init_info.cmd_buffer.end();

	vk::UniqueFence u_fence;
	if( !init_info.fence ){
		vk::FenceCreateInfo f_cr_inf;
		u_fence = context.device.device->createFenceUnique( f_cr_inf );
		init_info.fence = *u_fence;
	}

	std::array<vk::CommandBuffer, 1> cmd_bufs{ init_info.cmd_buffer };

	vk::SubmitInfo sub_inf( {}, {}, cmd_bufs, {} );

	if( vk::Result::eSuccess != init_info.queue.submit( 1, &sub_inf, init_info.fence )){
		KST_CORE_VERIFY( false, "Could not upload imgui fonts" );
	}

	KST_CORE_VERIFY( vk::Result::eSuccess == context.device.device->waitForFences( 1, &init_info.fence, VK_TRUE, UINT64_MAX ), "Wait for fence failed" );
	if( !u_fence ){
		KST_CORE_VERIFY( vk::Result::eSuccess == context.device.device->resetFences( 1, &init_info.fence ), "Could not reset fence" );
	}

	ImGui_ImplVulkan_DestroyFontUploadObjects();

	initialized = true;

	KST_CORE_INFO( "Created imgui context" );
}

KST_VK_ImguiWindowData::~KST_VK_ImguiWindowData(){
	if( initialized ){
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}
