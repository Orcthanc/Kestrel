#include "Platform/Vulkan/VKCameraRenderer.hpp"

#include "Scene/Components.hpp"
#include "Platform/Vulkan/VKMaterial.hpp"
#include "Platform/Vulkan/VKVertex.hpp"
#include "Renderer/CameraModes.hpp"
#include "Core/Application.hpp"
#include "Platform/Vulkan/VKMesh.hpp"

#include "imgui_impl_glfw.h"

#include <future>

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
	//allocMemory();
	createSynchronization();
	createImages();
	createImgui( *KST_VK_Context::get().device.renderpass );
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

#ifdef KST_COLOR_STATS
	sync.color_cpy_start = device_surface->device->createSemaphoreUnique( sem_inf );

	vk::FenceCreateInfo fence_inf;
	sync.color_cpy_done = device_surface->device->createFenceUnique( fence_inf );
#endif

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
		img_view_inf.format = vk::Format::eD32Sfloat;

		r.color_depth_view[1] = device_surface->device->createImageViewUnique( img_view_inf );
	}

#ifdef KST_COLOR_STATS
	auto buf_size = device_surface->swapchains[0].size.width * device_surface->swapchains[0].size.height * 4;
	vk::BufferCreateInfo buf_cr_inf(
			{},
			buf_size,
			vk::BufferUsageFlagBits::eTransferDst,
			vk::SharingMode::eExclusive,
			{}
		);

	copy_buffer.buffer = device_surface->device->createBufferUnique( buf_cr_inf );


	auto memory_reqs = device_surface->device->getBufferMemoryRequirements( *copy_buffer.buffer );
	vk::MemoryAllocateInfo mem_inf(
			memory_reqs.size,
			device_surface->find_memory_type(
				memory_reqs.memoryTypeBits,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent )
		);

	copy_buffer.memory = device_surface->device->allocateMemoryUnique( mem_inf );
	device_surface->device->bindBufferMemory( *copy_buffer.buffer, *copy_buffer.memory, 0 );
	copy_buffer.data = device_surface->device->mapMemory( *copy_buffer.memory, 0, buf_size );

#endif
}

void KST_VK_CameraRenderer::createImgui( vk::RenderPass render_pass ){
	vk::CommandBufferAllocateInfo alloc_inf( *transfer_cmd_pool, vk::CommandBufferLevel::ePrimary, 1 );
	auto temp = device_surface->device->allocateCommandBuffersUnique( alloc_inf );
	imgui.init( static_cast<KST_VK_Context&>( *Application::getInstance()->graphics_context ), Imgui_InitInfo( transfer_queue, render_pass, frames, *temp[0] ));


}

void KST_VK_CameraRenderer::onSizeChange( bool resetSync ){
	PROFILE_FUNCTION();
	device_surface->device->waitIdle();


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

	vk::CommandBufferAllocateInfo alloc_inf(
			*render_cmd_pool,
			vk::CommandBufferLevel::ePrimary,
			1
		);

	render_info.cmd_buffer = device_surface->device->allocateCommandBuffersUnique( alloc_inf );

	vk::CommandBufferBeginInfo beg_inf( {}, {} );
	render_info.cmd_buffer[0]->begin( beg_inf );

	vk::DeviceSize offset = 0;
	render_info.cmd_buffer[0]->bindVertexBuffers( 0, 1, &VK_MeshRegistry::mesh_data.vertex_buffer.buffer.get(), &offset );
	render_info.cmd_buffer[0]->bindIndexBuffer( *VK_MeshRegistry::mesh_data.index_buffer.buffer, 0, vk::IndexType::eUint32 );

	VK_ViewProj viewproj;

	viewproj.view = c.view;
	viewproj.proj = c.proj;
	viewproj.viewproj = c.proj * c.view;

	memcpy( uniform_buffer.data, &viewproj, sizeof( VK_ViewProj ));

	render_info.target = &render_targets.startPass();
}

void KST_VK_CameraRenderer::draw( Entity e ){
	PROFILE_FUNCTION();

	KST_CORE_ASSERT( e.hasComponent<TransformComponent>(), "Need a transform component to draw {}", e.getComponent<NameComponent>().name );
	KST_CORE_ASSERT( e.hasComponent<MeshComponent>(), "Need a mesh component to draw {}", e.getComponent<NameComponent>().name );
	KST_CORE_ASSERT( e.hasComponent<MaterialComponent>(), "Need a material component to draw {}", e.getComponent<NameComponent>().name );

	auto [transform, mesh, mat] = e.getComponents<TransformComponent, MeshComponent, MaterialComponent>();

	//Transform

	auto model = glm::scale( glm::translate( glm::identity<glm::mat4>(), transform.loc ) * glm::mat4_cast( transform.rot ), transform.scale );
	glm::vec3 color;
	if( e.hasComponent<ColorComponent>()){
		color = e.getComponent<ColorComponent>().color;
	} else{
		color = glm::vec3( 1.0, 1.0, 1.0 );
	}
	VK_UniformBufferObj mod_col{ model, color };
	render_info.cmd_buffer[0]->pushConstants( *VK_Materials::getInstance()[ mat ].layout, vk::ShaderStageFlagBits::eTessellationEvaluation, 0, sizeof( mod_col ), &mod_col );

	//Mat
	if( mat.mat != render_info.bound_mat ){
		if( render_info.bound_mat != -1 ){
			render_info.cmd_buffer[0]->endRenderPass();
		}
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
				current_id,
				views,
				render_targets.begin()->size,
				render_targets.getIndex(),
				*render_info.cmd_buffer[0],
				*uniform_buffer.buffer,
				render_info.render_mode );

		VK_Materials::getInstance()[ mat ].bind( bind_inf );

		render_info.bound_mat = mat.mat;
	}

	auto mimp = VK_MeshRegistry::getMeshImpl( mesh );

	// TODO clean up (maybe amount instead of size)
	render_info.cmd_buffer[0]->drawIndexed( mimp->index_amount, 1, mimp->index_offset, mimp->vertex_offset, 0 );
}

void KST_VK_CameraRenderer::endScene(){
	PROFILE_FUNCTION();

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData( ImGui::GetDrawData(), *render_info.cmd_buffer[0] );

	render_info.cmd_buffer[0]->endRenderPass();
	render_info.cmd_buffer[0]->end();


	//TODO check OutOfDate

	std::vector<vk::CommandBuffer> buffers{ *render_info.cmd_buffer[0] };
	std::vector signal_semas{
		*render_info.target->render_done_sema,
#ifdef KST_COLOR_STATS
		*sync.color_cpy_start,
#endif
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

	if (vk::Result::eSuccess != graphics_queue.submit(1, &sub_inf, {})) {
		throw std::runtime_error( "Error during queue_submit" );
	}


#ifdef KST_COLOR_STATS

	//Copy image to cpu
	vk::CommandBufferAllocateInfo al_inf(
			*transfer_cmd_pool,
			vk::CommandBufferLevel::ePrimary,
			1 );

	auto transferbuffer2 = device_surface->device->allocateCommandBuffersUnique( al_inf );
	{
		vk::CommandBufferBeginInfo beg_inf(
				vk::CommandBufferUsageFlagBits::eOneTimeSubmit, {});

		transferbuffer2[0]->begin( beg_inf );

		vk::BufferMemoryBarrier mem_bar(
				vk::AccessFlagBits::eHostRead,
				vk::AccessFlagBits::eTransferWrite,
				device_surface->queue_families.transfer.value(),
				device_surface->queue_families.transfer.value(),
				*copy_buffer.buffer,
				0,
				VK_WHOLE_SIZE );

		transferbuffer2[0]->pipelineBarrier(
				vk::PipelineStageFlagBits::eHost,
				vk::PipelineStageFlagBits::eTransfer,
				{},
				0, nullptr,
				1, &mem_bar,
				0, nullptr );

		vk::BufferImageCopy img_cpy(
				0,
				0,
				0,
				{ vk::ImageAspectFlagBits::eColor, 0, 0, 1 },
				{ 0, 0, 0 },
				{
					render_targets[ render_targets.getIndex() ].size.width,
					render_targets[ render_targets.getIndex() ].size.height,
					1
				}
			);

		transferbuffer2[0]->copyImageToBuffer(
				*render_info.target->color_depth[0],
				vk::ImageLayout::eTransferSrcOptimal,
				*copy_buffer.buffer,
				1, &img_cpy );

		transferbuffer2[0]->end();

		std::array wait_sema{ *sync.color_cpy_start };
		std::array<vk::PipelineStageFlags, 1> wait_stages{ vk::PipelineStageFlagBits::eTransfer };

		vk::SubmitInfo sub_inf2(
				wait_sema,
				wait_stages,
				*transferbuffer2[0],
				{} );

		KST_CORE_VERIFY( vk::Result::eSuccess == transfer_queue.submit( 1, &sub_inf2, *sync.color_cpy_done ), "Queue submit failed in line {}", __LINE__ );
	}
#endif



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

#ifdef KST_COLOR_STATS
		KST_CORE_VERIFY( vk::Result::eSuccess == device_surface->device->waitForFences( 1, &*sync.color_cpy_done, VK_TRUE, UINT64_MAX ), "Wait for fence failed in line {}", __LINE__ );

		KST_CORE_VERIFY( vk::Result::eSuccess == device_surface->device->resetFences( 1, &*sync.color_cpy_done ), "Wait for fence failed in line {}", __LINE__ );

		std::unordered_map<uint32_t, uint32_t> colors;
		std::vector<std::future<std::unordered_map<uint32_t, uint32_t>>> futures;

		size_t max_size = render_info.target->size.width * render_info.target->size.height;
		constexpr size_t threads = 8;
		size_t step = max_size / threads;

		for( size_t i = 0; i < threads; ++i ){
			futures.emplace_back( std::async( [max_size, step]( size_t id, void* data ){
					std::unordered_map<uint32_t, uint32_t> colors2;
					for( size_t i = id * step; i < ( id == threads - 1 ? max_size : ( id + 1 ) * step ); ++i ){
						++colors2[ reinterpret_cast<uint32_t*>( data )[i]];
					}
					return colors2;
				}, i, copy_buffer.data ));
			std::unordered_map<uint32_t, uint32_t> colors2;
		}

		for( auto& f: futures ){
			for( auto& [key, value]: f.get()){
				colors[key] += value;
			}
		}

		static bool past_crit = false;
		static size_t counter = 0;
		if( colors.contains( 0xff00ff00 ) || colors.contains( 0xff000000 ) || colors.contains( 0x00000000 )){
			if( past_crit )
				++counter;
		} else
			past_crit = true;

		if( counter >= 100 )
			Application::getInstance()->running = false;

		for( auto& [key, value]: colors ){
			color_stat_file << std::hex << key << ':' << std::dec << value << " ";
		}
		color_stat_file << "\n";
#endif

		//TODO remove
		present_queue.waitIdle();
	}
}
