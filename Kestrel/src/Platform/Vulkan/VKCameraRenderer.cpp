#include "Platform/Vulkan/VKMesh.hpp"

#include "Platform/Vulkan/VKCameraRenderer.hpp"

#include "Core/Application.hpp"

#include "Renderer/CameraModes.hpp"

#include "Scene/Components.hpp"

#include "Platform/Vulkan/VKMaterial.hpp"
#include "Platform/Vulkan/VKVertex.hpp"
#include "Platform/Vulkan/VKTerrainRenderer.hpp"
#include "Platform/Vulkan/VKTerrain.hpp"

#include "imgui_impl_glfw.h"

#include <chrono>

using namespace Kestrel;

KST_VK_CameraRenderer::KST_VK_CameraRenderer( KST_VK_DeviceSurface* surface ){
	PROFILE_FUNCTION();

	setDeviceSurface( std::move( surface ));
}

void KST_VK_CameraRenderer::setDeviceSurface( KST_VK_DeviceSurface* surface ){
	PROFILE_FUNCTION();

	KST_CORE_ASSERT( !device_surface, "Can not assign device surface multiple times" );

	device_surface = surface;

	vk::CommandPoolCreateInfo cmd_pl_inf(
			vk::CommandPoolCreateFlagBits::eTransient,
			device_surface->queue_families.graphics.value()
		);

	render_cmd_pool = device_surface->device->createCommandPoolUnique( cmd_pl_inf );

	cmd_pl_inf.queueFamilyIndex = device_surface->queue_families.transfer.value();

	transfer_cmd_pool = device_surface->device->createCommandPoolUnique( cmd_pl_inf );

	graphics_queue = device_surface->device->getQueue( device_surface->queue_families.graphics.value(), 0 );
	present_queue = device_surface->device->getQueue( device_surface->queue_families.present.value(), 0 );
	transfer_queue = device_surface->device->getQueue( device_surface->queue_families.transfer.value(), 0 );

	createBuffers();
	createSynchronization();
	createImages();
	createImgui( *KST_VK_Context::get().device.renderpass );

	KST_VK_TerrainRenderer::init();
}

void KST_VK_CameraRenderer::createBuffers(){
	PROFILE_FUNCTION();

	KST_VK_BufferCreateInfo buf_cr_inf(
			device_surface,
			sizeof( VK_ViewProj ),
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent );

	uniform_buffer.create( buf_cr_inf );
	uniform_buffer.map();
}

void KST_VK_CameraRenderer::createSynchronization(){
	PROFILE_FUNCTION();

	vk::SemaphoreCreateInfo sem_inf;
	sync.start_rendering = device_surface->device->createSemaphoreUnique( sem_inf );
	sync.image_presentable = device_surface->device->createSemaphoreUnique( sem_inf );

	vk::FenceCreateInfo fence_cr_inf( vk::FenceCreateFlagBits::eSignaled );

	for( auto& r: render_targets ){
		r.render_ready_sema = device_surface->device->createSemaphoreUnique( sem_inf );
		r.render_done_sema = device_surface->device->createSemaphoreUnique( sem_inf );
		r.render_done_fence = device_surface->device->createFenceUnique( fence_cr_inf );
	}
}

void KST_VK_CameraRenderer::createImages(){
	PROFILE_FUNCTION();

	std::set<uint32_t> queue_families;

	queue_families.insert( device_surface->queue_families.graphics.value() );
	queue_families.insert( device_surface->queue_families.present.value() );

	std::vector<uint32_t> queue_families_v( queue_families.begin(), queue_families.end() );

	//TODO replace with KST_VK_Image_DeviceLocal

	//Images
	vk::ImageCreateInfo img_inf(
			{},
			vk::ImageType::e2D,
			vk::Format::eB8G8R8A8Srgb,
			{ device_surface->swapchains[0].size.width, device_surface->swapchains[0].size.height, 1 },
			1, 1,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
			vk::SharingMode::eExclusive,
			queue_families_v,
			vk::ImageLayout::eUndefined
		);

	for( auto& r: render_targets ){
		r.color_depth[0] = device_surface->device->createImageUnique( img_inf );
		r.size = device_surface->swapchains[0].size;
	}

	if( integer_db )
		img_inf.format = vk::Format::eD24UnormS8Uint;
	else
		img_inf.format = vk::Format::eD32Sfloat;

	img_inf.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;

	for( auto& r: render_targets ){
		r.color_depth[1] = device_surface->device->createImageUnique( img_inf );
	}

	//Memory
	for( auto& r: render_targets ){
		auto mem_reqs = device_surface->device->getImageMemoryRequirements( *r.color_depth[0] );
		vk::MemoryAllocateInfo mem_inf(
				mem_reqs.size,
				device_surface->find_memory_type(
					mem_reqs.memoryTypeBits,
					vk::MemoryPropertyFlagBits::eDeviceLocal )
			);

		r.color_depth_mem[0] = device_surface->device->allocateMemoryUnique( mem_inf );
		device_surface->device->bindImageMemory( *r.color_depth[0], *r.color_depth_mem[0], 0 );

		mem_reqs = device_surface->device->getImageMemoryRequirements( *r.color_depth[1] );
		mem_inf = vk::MemoryAllocateInfo(
				mem_reqs.size,
				device_surface->find_memory_type(
					mem_reqs.memoryTypeBits,
					vk::MemoryPropertyFlagBits::eDeviceLocal )
			);

		r.color_depth_mem[1] = device_surface->device->allocateMemoryUnique( mem_inf );
		device_surface->device->bindImageMemory( *r.color_depth[1], *r.color_depth_mem[1], 0 );
	}

	//Views
	vk::ImageViewCreateInfo img_view_inf(
			{},
			{}, //Set in loop
			vk::ImageViewType::e2D,
			{}, //Set in loop
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
				{}, //Set in loop
				0, 1,
				0, 1 )
		);

	for( auto& r: render_targets ){
		img_view_inf.image = *r.color_depth[0];
		img_view_inf.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		img_view_inf.format = vk::Format::eB8G8R8A8Srgb;

		r.color_depth_view[0] = device_surface->device->createImageViewUnique( img_view_inf );

		img_view_inf.image = *r.color_depth[1];
		img_view_inf.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

		if( integer_db )
			img_view_inf.format = vk::Format::eD24UnormS8Uint;
		else
			img_view_inf.format = vk::Format::eD32Sfloat;

		r.color_depth_view[1] = device_surface->device->createImageViewUnique( img_view_inf );

		std::vector<vk::ImageView> attachments{
			*r.color_depth_view[0],
			*r.color_depth_view[1]
		};

		vk::FramebufferCreateInfo framebuf_inf(
			{},
			*KST_VK_Context::get().device.renderpass,
			attachments,
			device_surface->swapchains[0].size.width,
			device_surface->swapchains[0].size.height,
			1
		);

		r.framebuffer = device_surface->device->createFramebufferUnique( framebuf_inf );
	}
}

void KST_VK_CameraRenderer::createImgui( vk::RenderPass render_pass ){
	vk::CommandBufferAllocateInfo alloc_inf( *transfer_cmd_pool, vk::CommandBufferLevel::ePrimary, 1 );
	auto temp = device_surface->device->allocateCommandBuffersUnique( alloc_inf );
	imgui.init( static_cast<KST_VK_Context&>( *Application::getInstance()->graphics_context ), Imgui_InitInfo( transfer_queue, render_pass, frames, *temp[0] ));


}

void KST_VK_CameraRenderer::onSizeChange( bool resetSync ){
	PROFILE_FUNCTION();
	device_surface->device->waitIdle();

	createImgui( *KST_VK_Context::get().device.renderpass );

	device_surface->swapchains[ render_info.window_index ].create(
			(*device_surface->windows)[ render_info.window_index ].surface,
			*device_surface );


	if( resetSync )
		createSynchronization();
	createImages();

	++current_id;
}

void KST_VK_CameraRenderer::begin_scene( Camera& c, size_t window_index ){
	PROFILE_FUNCTION();

	render_info = {};

	render_info.window_index = window_index;

	render_info.render_mode = c.camera_render_mode;

	render_info.tessellation = 1;

	vk::CommandBufferAllocateInfo alloc_inf(
			*render_cmd_pool,
			vk::CommandBufferLevel::ePrimary,
			1
		);

	render_info.cmd_buffer = device_surface->device->allocateCommandBuffersUnique( alloc_inf );

	vk::CommandBufferBeginInfo beg_inf( {}, {} );
	render_info.cmd_buffer[0]->begin( beg_inf );

	std::array<vk::Buffer, 1> buffers{
		VK_MeshRegistry::shared_resources.mesh_data.vertex_buffer.buffer.get(),
	};

	std::array<vk::DeviceSize, 1> offsets{
		0,
	};

	vk::DeviceSize offset = 0;
	render_info.cmd_buffer[0]->bindVertexBuffers( 0, buffers, offsets );
	render_info.cmd_buffer[0]->bindIndexBuffer( *VK_MeshRegistry::shared_resources.mesh_data.index_buffer.buffer, 0, vk::IndexType::eUint32 );

	view_proj.view = c.view;
	view_proj.proj = c.proj;
	view_proj.viewproj = c.proj * c.view;

	memcpy( uniform_buffer.data, &view_proj, sizeof( VK_ViewProj ));

	render_info.target = &render_targets.startPass();

	std::array<vk::ClearValue, 2> clear_values{
		vk::ClearColorValue(std::array<float, 4>{ 0.0, 0.0, 0.0, 0.0 }),
		vk::ClearDepthStencilValue( 1.0f, 0 )
	};

	if( any_flag( render_info.render_mode & RenderModeFlags::eInverse )){
		clear_values[1] = vk::ClearDepthStencilValue( 0.0f, 0 );
	}

	vk::RenderPassBeginInfo render_beg_inf(
			*KST_VK_Context::get().device.renderpass,
			*render_info.target->framebuffer,
			{{ 0, 0 }, render_info.target->size },
			clear_values
		);

	render_info.cmd_buffer[0]->beginRenderPass( render_beg_inf, vk::SubpassContents::eInline );
}

void KST_VK_CameraRenderer::draw( Entity e ){
	if( e.hasComponent<MeshComponent>()){
		auto [transform, mesh, mat] = e.getComponents<TransformComponent, MeshComponent, MaterialComponent>();
		glm::vec3 color{ 1, 1, 1 };
		if( e.hasComponent<ColorComponent>())
			color = e.getComponent<ColorComponent>();
		drawMesh( transform, mesh, mat, 1, color );
	}
	if( e.hasComponent<TerrainComponent>()){
		auto [transform, terrain] = e.getComponents<TransformComponent, TerrainComponent>();
		KST_VK_TerrainRenderer::get().drawTerrain( this, transform, terrain );
	}

	//KST_CORE_INFO( "Drawing {}", e.getComponent<NameComponent>().name );
}

void KST_VK_CameraRenderer::drawMesh( const TransformComponent& transform, const Mesh& mesh, const Material& mat, float tessellation, const glm::vec3& color ){
	PROFILE_FUNCTION();

	//Transform
	auto model = glm::scale( glm::translate( glm::identity<glm::mat4>(), transform.loc ) * glm::mat4_cast( transform.rot ), transform.scale );

	VK_UniformBufferObj mod_col{ model, color };
	render_info.cmd_buffer[0]->pushConstants(
			*VK_Materials::requestResource( mat )->layout,
			vk::ShaderStageFlagBits::eTessellationEvaluation,
			0,
			sizeof( mod_col ),
			&mod_col );

	render_info.cmd_buffer[0]->pushConstants(
			*VK_Materials::requestResource( mat )->layout,
			vk::ShaderStageFlagBits::eTessellationControl,
			80,
			4,
			&render_info.tessellation );

	//Mat
	if( mat != render_info.bound_mat ){
		std::vector<std::array<vk::ImageView, 2>> views;

		views.reserve( render_targets.size );

		for( auto& t: render_targets ){
			views.push_back(std::array<vk::ImageView, 2>{
					*t.color_depth_view[0],
					*t.color_depth_view[1] });
		}

		BindingInfo bind_inf(
				*device_surface,
				this,
				render_targets.begin()->size,
				render_targets.getIndex(),
				*render_info.cmd_buffer[0],
				*uniform_buffer.buffer,
				render_info.render_mode );

		VK_Materials::requestResource( mat )->bind( bind_inf );

		render_info.bound_mat = mat;
	}

	auto mimp = VK_MeshRegistry::requestResource( mesh );

	// TODO clean up (maybe amount instead of size)
	render_info.cmd_buffer[0]->drawIndexed(
			static_cast<uint32_t>( mimp->index_amount ), 1,
			static_cast<uint32_t>( mimp->index_offset ),
			static_cast<int32_t>( mimp->vertex_offset ), 0 );
}

void KST_VK_CameraRenderer::endScene(){
	PROFILE_FUNCTION();

	if( render_info.bound_mat == -1 ){
		KST_CORE_ERROR( "Can not draw scene with no valid objects" );
		render_info.cmd_buffer[0]->end();
		ImGui::EndFrame();
		return;
	}

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData( ImGui::GetDrawData(), *render_info.cmd_buffer[0] );

	render_info.cmd_buffer[0]->endRenderPass();
	render_info.cmd_buffer[0]->end();

	std::vector<vk::CommandBuffer> buffers{ *render_info.cmd_buffer[0] };
	std::vector signal_semas{
		*render_info.target->render_done_sema,
	};

	vk::SubmitInfo sub_inf(
			{},
			{},
			buffers,
			signal_semas );

	if( vk::Result::eSuccess != device_surface->device->waitForFences( 1, &*render_info.target->render_done_fence, true, UINT64_MAX ))
		KST_CORE_VERIFY( false, "Could not wait on fence" );

	if( vk::Result::eSuccess != device_surface->device->resetFences( 1, &*render_info.target->render_done_fence )){
		KST_CORE_VERIFY( false, "Could not reset fence" );
	}

	graphics_queue.waitIdle();
	auto start_t = std::chrono::steady_clock::now();


	if (vk::Result::eSuccess != graphics_queue.submit(1, &sub_inf, {})) {
		throw std::runtime_error( "Error during queue_submit" );
	}

	graphics_queue.waitIdle();

	auto end_t = std::chrono::steady_clock::now();

	frame_times << std::chrono::duration_cast<std::chrono::microseconds>( end_t - start_t ).count() << std::endl;

	auto img_res = device_surface->device->acquireNextImageKHR(
			*device_surface->swapchains[render_info.window_index].swapchain,
			UINT64_MAX,
			*sync.start_rendering,
			{} );

	if( img_res.result == vk::Result::eErrorOutOfDateKHR ){
		onSizeChange( true );
		return;
	}

	uint32_t img_index = img_res.value;
	{
		//Copy image to swapchain
		vk::CommandBufferAllocateInfo al_inf(
				*transfer_cmd_pool,
				vk::CommandBufferLevel::ePrimary,
				1 );

		auto transferbuffer = device_surface->device->allocateCommandBuffersUnique( al_inf );

		vk::CommandBufferBeginInfo beg_inf(
				vk::CommandBufferUsageFlagBits::eOneTimeSubmit, {});

		transferbuffer[0]->begin( beg_inf );

		VK_Utils::changeImageLayout(
				*transferbuffer[0],
				device_surface->swapchains[render_info.window_index].images[img_index],
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eTransferDstOptimal,
				vk::ImageSubresourceRange( vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 ),
				vk::PipelineStageFlagBits::eTopOfPipe,
				vk::PipelineStageFlagBits::eTransfer );

		vk::ImageCopy img_copy(
				vk::ImageSubresourceLayers(
					vk::ImageAspectFlagBits::eColor,
					0, 0, 1 ),
				vk::Offset3D( 0, 0, 0 ),
				vk::ImageSubresourceLayers(
					vk::ImageAspectFlagBits::eColor,
					0, 0, 1 ),
				vk::Offset3D( 0, 0, 0 ),
				vk::Extent3D(
					render_info.target->size.width,
					render_info.target->size.height,
					1 )
			);

		transferbuffer[0]->copyImage(
				*render_info.target->color_depth[0],
				vk::ImageLayout::eTransferSrcOptimal,
				device_surface->swapchains[render_info.window_index].images[img_index],
				vk::ImageLayout::eTransferDstOptimal,
				1, &img_copy );

		VK_Utils::changeImageLayout(
				*transferbuffer[0],
				device_surface->swapchains[render_info.window_index].images[img_index],
				vk::ImageLayout::eTransferDstOptimal,
				vk::ImageLayout::ePresentSrcKHR,
				vk::ImageSubresourceRange( vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 ),
				vk::PipelineStageFlagBits::eTransfer,
				vk::PipelineStageFlagBits::eBottomOfPipe );

		transferbuffer[0]->end();

		std::array<vk::PipelineStageFlags, 2> flags = {
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eTransfer
		};

		std::array<vk::Semaphore, 1> copy_done_semas = {
			*sync.image_presentable,
		};

		std::array<vk::Semaphore, 2> pre_copy_semas = {
			*render_info.target->render_done_sema,
			*sync.start_rendering
		};

		vk::SubmitInfo submit_inf(
				2, pre_copy_semas.data(),
				flags.data(),
				1, &*transferbuffer[0],
				1, copy_done_semas.data() );

		if( vk::Result::eSuccess != transfer_queue.submit( 1, &submit_inf, *render_info.target->render_done_fence )){
			KST_CORE_ERROR( "Rendering failed" );
			throw std::runtime_error( "Rendering failed" );
		}

		std::vector swapchains{ *device_surface->swapchains[render_info.window_index].swapchain };
		std::vector indices{ img_index };
		std::vector ready_semas{ *sync.image_presentable };
		vk::PresentInfoKHR pres_inf(
				ready_semas,
				swapchains,
				indices,
				{} );

		try {
			switch( present_queue.presentKHR( pres_inf )){
				case vk::Result::eErrorOutOfDateKHR:
				case vk::Result::eSuboptimalKHR:
					onSizeChange( false );
					break;
				default:
					break;
			}
		} catch( vk::OutOfDateKHRError e ){
			onSizeChange( false );
		}

		//TODO remove
		present_queue.waitIdle();
	}
}
