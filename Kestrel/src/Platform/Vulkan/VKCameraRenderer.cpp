#include "Platform/Vulkan/VKCameraRenderer.hpp"

#include "Scene/Components.hpp"
#include "Platform/Vulkan/VKMaterial.hpp"
#include "Platform/Vulkan/VKVertex.hpp"

using namespace Kestrel;

KST_VK_CameraRenderer::KST_VK_CameraRenderer( KST_VK_DeviceSurface* surface ){
	PROFILE_FUNCTION();
	setDeviceSurface( std::move( surface ));
}

void KST_VK_CameraRenderer::setDeviceSurface( KST_VK_DeviceSurface* surface ){
	PROFILE_FUNCTION();

	KST_INFO( "Created device surface" );

	KST_CORE_ASSERT( !device_surface, "Can not assign device surface multiple times" );

	device_surface = surface;

	vk::CommandPoolCreateInfo cmd_pl_inf(
			vk::CommandPoolCreateFlagBits::eTransient,
			device_surface->queue_families.graphics.value()
		);

	render_cmd_pool = device_surface->device->createCommandPoolUnique( cmd_pl_inf );

	cmd_pl_inf.queueFamilyIndex = device_surface->queue_families.transfer.value();

	transfer_cmd_pool = device_surface->device->createCommandPoolUnique( cmd_pl_inf );

	// Buffers
	{
		vk::BufferCreateInfo buf_cr_inf(
				{},
				vert_buf_size,
				vk::BufferUsageFlagBits::eVertexBuffer,
				vk::SharingMode::eExclusive,
				{}
			);

		vertex_buffer.buffer = device_surface->device->createBufferUnique( buf_cr_inf );

		buf_cr_inf.size = index_buf_size;
		buf_cr_inf.usage = vk::BufferUsageFlagBits::eIndexBuffer;

		index_buffer.buffer = device_surface->device->createBufferUnique( buf_cr_inf );

		buf_cr_inf.size = sizeof( VK_ViewProj );
		buf_cr_inf.usage = vk::BufferUsageFlagBits::eUniformBuffer;

		uniform_buffer.buffer = device_surface->device->createBufferUnique( buf_cr_inf );
	}

	// Memory
	{
		auto memory_reqs = device_surface->device->getBufferMemoryRequirements( *vertex_buffer.buffer );
		vk::MemoryAllocateInfo mem_inf(
				memory_reqs.size,
				device_surface->find_memory_type(
					memory_reqs.memoryTypeBits,
					vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent )
			);

		vertex_buffer.memory = device_surface->device->allocateMemoryUnique( mem_inf );
		device_surface->device->bindBufferMemory( *vertex_buffer.buffer, *vertex_buffer.memory, 0 );
		vertex_buffer.data = device_surface->device->mapMemory( *vertex_buffer.memory, 0, vert_buf_size );

		memory_reqs = device_surface->device->getBufferMemoryRequirements( *index_buffer.buffer );
		mem_inf.allocationSize = memory_reqs.size;
		mem_inf.memoryTypeIndex = device_surface->find_memory_type(
				memory_reqs.memoryTypeBits,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent );

		index_buffer.memory = device_surface->device->allocateMemoryUnique( mem_inf );
		device_surface->device->bindBufferMemory( *index_buffer.buffer, *index_buffer.memory, 0 );
		index_buffer.data = device_surface->device->mapMemory( *index_buffer.memory, 0, index_buf_size );

		memory_reqs = device_surface->device->getBufferMemoryRequirements( *uniform_buffer.buffer );
		mem_inf.allocationSize = memory_reqs.size;
		mem_inf.memoryTypeIndex = device_surface->find_memory_type(
				memory_reqs.memoryTypeBits,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent );

		uniform_buffer.memory = device_surface->device->allocateMemoryUnique( mem_inf );
		device_surface->device->bindBufferMemory( *uniform_buffer.buffer, *uniform_buffer.memory, 0 );
		uniform_buffer.data = device_surface->device->mapMemory( *uniform_buffer.memory, 0, sizeof( VK_ViewProj ));
	}

	graphics_queue = device_surface->device->getQueue( device_surface->queue_families.graphics.value(), 0 );
	present_queue = device_surface->device->getQueue( device_surface->queue_families.present.value(), 0 );
	transfer_queue = device_surface->device->getQueue( device_surface->queue_families.transfer.value(), 0 );

	//Sync
	{
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

	std::set<uint32_t> queue_families;

	queue_families.insert( device_surface->queue_families.graphics.value() );
	queue_families.insert( device_surface->queue_families.present.value() );

	std::vector<uint32_t> queue_families_v( queue_families.begin(), queue_families.end() );

	//Images
	{
		//Base
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
	}
}

void KST_VK_CameraRenderer::begin_scene( Camera& c, size_t window_index ){
	PROFILE_FUNCTION();

	render_info = {};

	render_info.window_index = window_index;

	vertex_buffer.current_offset = 0;
	index_buffer.current_offset = 0;

	vk::CommandBufferAllocateInfo alloc_inf(
			*render_cmd_pool,
			vk::CommandBufferLevel::ePrimary,
			1
		);

	render_info.cmd_buffer = device_surface->device->allocateCommandBuffersUnique( alloc_inf );

	vk::CommandBufferBeginInfo beg_inf( {}, {} );
	render_info.cmd_buffer[0]->begin( beg_inf );

	vk::DeviceSize offset = 0;
	render_info.cmd_buffer[0]->bindVertexBuffers( 0, 1, &vertex_buffer.buffer.get(), &offset );
	render_info.cmd_buffer[0]->bindIndexBuffer( *index_buffer.buffer, 0, vk::IndexType::eUint32 );

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

	auto [transform, mesh, mat] = e.getComponent<TransformComponent, MeshComponent, MaterialComponent>();

	//Transform

	auto model = glm::scale( glm::translate( glm::identity<glm::mat4>(), transform.loc ) * glm::mat4_cast( transform.rot ), transform.scale );

	render_info.cmd_buffer[0]->pushConstants( *VK_Materials::getInstance()[ mat ].layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof( model ), &model );

	//Mat
	if( mat.mat != render_info.bound_mat ){
		if( render_info.bound_mat != -1 ){
			render_info.cmd_buffer[0]->endRenderPass();
		}
		bindMat( VK_Materials::getInstance()[ mat ]);
	}

	auto verts = mesh.mesh->getVertices();
	auto indices = mesh.mesh->getIndices();

	//TODO what if buffers overflow
	memcpy( reinterpret_cast<uint8_t*>( vertex_buffer.data ) + vertex_buffer.current_offset, verts.data, verts.size );

	memcpy( reinterpret_cast<uint8_t*>( index_buffer.data ) + index_buffer.current_offset, indices.data, indices.size );

	// TODO clean up (maybe amount instead of size)
	render_info.cmd_buffer[0]->drawIndexed( indices.size / indices.elem_size, 1, index_buffer.current_offset / indices.elem_size, vertex_buffer.current_offset / verts.elem_size, 0 );

	vertex_buffer.current_offset += verts.size;
	index_buffer.current_offset += indices.size;
}

void KST_VK_CameraRenderer::endScene(){
	render_info.cmd_buffer[0]->endRenderPass();
	render_info.cmd_buffer[0]->end();


//TODO check OutOfDate

	std::vector<vk::Semaphore> wait_semas{ *render_info.target->render_ready_sema };
	std::vector<vk::Flags<vk::PipelineStageFlagBits>> wait_stages{ vk::PipelineStageFlagBits::eColorAttachmentOutput };

	std::vector<vk::CommandBuffer> buffers{ *render_info.cmd_buffer[0] };
	std::vector signal_semas{ *render_info.target->render_done_sema };

	vk::SubmitInfo sub_inf(
			wait_semas,
			wait_stages,
			buffers,
			signal_semas );

	//TODO
	if( vk::Result::eSuccess != graphics_queue.submit( 1, &sub_inf )){
		throw std::runtime_error( "Error during queue_submit" );
	}

	auto img_res = device_surface->device->acquireNextImageKHR(
			*device_surface->swapchains[render_info.window_index].swapchain,
			UINT64_MAX,
			*sync.start_rendering );

	uint32_t img_index = img_res.value;
	{
		//Copy image to swapchain
		vk::CommandBufferAllocateInfo al_inf(
				*transfer_cmd_pool,
				vk::CommandBufferLevel::ePrimary,
				1 );

		auto transferbuffer = device_surface->device->allocateCommandBuffersUnique( al_inf );

		vk::CommandBufferBeginInfo beg_inf(
				vk::CommandBufferUsageFlagBits::eOneTimeSubmit, {}  );

		transferbuffer[0]->begin( beg_inf );

		vk::ImageMemoryBarrier img_barrier(
				vk::AccessFlagBits::eColorAttachmentWrite,
				vk::AccessFlagBits::eTransferRead,
				vk::ImageLayout::eColorAttachmentOptimal,
				vk::ImageLayout::eTransferSrcOptimal,
				device_surface->queue_families.graphics.value(),
				device_surface->queue_families.transfer.value(),
				*render_info.target->color_depth[0],
				vk::ImageSubresourceRange( vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 ));

		transferbuffer[0]->pipelineBarrier(
				vk::PipelineStageFlagBits::eColorAttachmentOutput,
				vk::PipelineStageFlagBits::eTransfer,
				{},
				0, nullptr,
				0, nullptr,
				1, &img_barrier );


		vk::ImageMemoryBarrier swap_barrier1(
				vk::AccessFlagBits::eMemoryRead,
				vk::AccessFlagBits::eTransferWrite,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eTransferDstOptimal,
				device_surface->queue_families.present.value(),
				device_surface->queue_families.transfer.value(),
				device_surface->swapchains[render_info.window_index].images[img_index],
				vk::ImageSubresourceRange( vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 )
			);

		transferbuffer[0]->pipelineBarrier(
				vk::PipelineStageFlagBits::eTopOfPipe,
				vk::PipelineStageFlagBits::eTransfer,
				{},
				0, nullptr,
				0, nullptr,
				1, &swap_barrier1 );

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

		vk::ImageMemoryBarrier swap_barrier2(
				vk::AccessFlagBits::eTransferWrite,
				vk::AccessFlagBits::eMemoryRead,
				vk::ImageLayout::eTransferDstOptimal,
				vk::ImageLayout::ePresentSrcKHR,
				device_surface->queue_families.transfer.value(),
				device_surface->queue_families.present.value(),
				device_surface->swapchains[render_info.window_index].images[img_index],
				vk::ImageSubresourceRange( vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 )
			);

		transferbuffer[0]->pipelineBarrier(
				vk::PipelineStageFlagBits::eTransfer,
				vk::PipelineStageFlagBits::eBottomOfPipe,
				{},
				0, nullptr,
				0, nullptr,
				1, & swap_barrier2 );

		transferbuffer[0]->end();

		vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eTransfer;

		std::array<vk::Semaphore, 2> copy_done_semas = {
			*sync.image_presentable,
			*render_info.target->render_ready_sema
		};

		vk::SubmitInfo submit_inf(
				1, &*render_info.target->render_done_sema,
				&flags,
				1, &*transferbuffer[0],
				2, copy_done_semas.data() );

		if( vk::Result::eSuccess != transfer_queue.submit( 1, &submit_inf )){
			KST_CORE_ERROR( "Rendering failed" );
			throw std::runtime_error( "Rendering failed" );
		}

		transfer_queue.waitIdle();
	}

	std::vector swapchains{ *device_surface->swapchains[render_info.window_index].swapchain };
	std::vector indices{ img_index };
	std::vector ready_semas{ *sync.image_presentable };
	vk::PresentInfoKHR pres_inf(
			ready_semas,
			swapchains,
			indices,
			{} );

	switch( present_queue.presentKHR( pres_inf )){
		case vk::Result::eErrorOutOfDateKHR:
		case vk::Result::eSuboptimalKHR:
			//TODO
			break;
		default:
			break;
	}

	present_queue.waitIdle();
}

void KST_VK_CameraRenderer::bindMat( VK_Material_T& mat ){
	PROFILE_FUNCTION();

	vk::DescriptorSetAllocateInfo desc_set_alloc_inf(
			*mat.desc_pool,
			1,
			&*mat.desc_layout );

	if( !mat.desc_sets.contains( this ))
		mat.desc_sets.emplace( this, device_surface->device->allocateDescriptorSetsUnique( desc_set_alloc_inf ));

	auto& desc_sets = mat.desc_sets.at( this );

	std::array<vk::DescriptorBufferInfo, 1> desc_buf_info{
		vk::DescriptorBufferInfo( *uniform_buffer.buffer,
			0,
			sizeof( VK_ViewProj ))
	};

	vk::WriteDescriptorSet write_desc(
			*desc_sets[0],
			0, 0,
			vk::DescriptorType::eUniformBuffer,
			{},
			desc_buf_info,
			{}
		);

	device_surface->device->updateDescriptorSets( 1, &write_desc, 0, nullptr );

	vk::Framebuffer buf;

	if( !mat.framebuffers.contains( this )){
		mat.framebuffers[ this ] = std::vector<vk::UniqueFramebuffer>();
		auto& framebufs = mat.framebuffers[this];

		framebufs.resize( frames );

		for( size_t i = 0; i < framebufs.size(); ++i ){

			std::vector<vk::ImageView> attachments{ *render_targets[i].color_depth_view[0] };

			vk::FramebufferCreateInfo framebuf_inf(
					{},
					*mat.renderpass,
					attachments,
					render_targets[i].size.width,
					render_targets[i].size.height,
					1
				);

			framebufs[i] = device_surface->device->createFramebufferUnique( framebuf_inf );

		}

		buf = *framebufs[ render_targets.getIndex() ];
	} else {
		buf = *mat.framebuffers.at( this )[ render_targets.getIndex() ];
	}


	vk::RenderPassBeginInfo beg_inf(
			*mat.renderpass,
			buf,
			{{ 0, 0 }, device_surface->swapchains[0].size }, //TODO
			{}
		);

	render_info.cmd_buffer[0]->beginRenderPass( beg_inf, vk::SubpassContents::eInline );
	render_info.cmd_buffer[0]->bindPipeline( vk::PipelineBindPoint::eGraphics, *mat.pipeline );

	auto [x, y] = (*device_surface->windows)[render_info.window_index].getResolution();

	vk::Viewport viewport(
			0, 0,
			x, y,
			0, 1 );

	render_info.cmd_buffer[0]->setViewport( 0, 1, &viewport );

	vk::Rect2D scissor(
			{ 0, 0 },
			{ x, y });

	render_info.cmd_buffer[0]->setScissor( 0, 1, &scissor );

	render_info.cmd_buffer[0]->bindDescriptorSets( vk::PipelineBindPoint::eGraphics, *mat.layout, 0, *desc_sets[0], {} );
}
