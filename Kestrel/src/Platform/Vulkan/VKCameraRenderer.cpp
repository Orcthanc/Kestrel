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

	KST_CORE_ASSERT( !device_surface, "Can not assign device surface multiple times" );

	device_surface = surface;

	vk::CommandPoolCreateInfo cmd_pl_inf(
			vk::CommandPoolCreateFlagBits::eTransient,
			device_surface->queue_families.graphics.value()
		);

	render_cmd_pool = device_surface->device->createCommandPoolUnique( cmd_pl_inf );

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
		//TODO shared memory
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

	vk::SemaphoreCreateInfo sem_inf;
	sync.start_rendering = device_surface->device->createSemaphoreUnique( sem_inf );
	sync.image_presentable = device_surface->device->createSemaphoreUnique( sem_inf );
}

void KST_VK_CameraRenderer::begin_scene( Camera& c, size_t window_index ){
	PROFILE_FUNCTION();

	render_info = {};

	render_info.window_index = window_index;

	//TODO render to offscreen buffer
	//TODO check result
	auto img_res = device_surface->device->acquireNextImageKHR(
			*device_surface->swapchains[render_info.window_index].swapchain,
			UINT64_MAX,
			*sync.start_rendering );

	render_info.img_index = img_res.value;


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

	std::vector wait_semas{ *sync.start_rendering };
	std::vector<vk::Flags<vk::PipelineStageFlagBits>> wait_stages{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
	std::vector<vk::CommandBuffer> buffers{ *render_info.cmd_buffer[0] };
	std::vector signal_semas{ *sync.image_presentable };

	vk::SubmitInfo sub_inf(
			wait_semas,
			wait_stages,
			buffers,
			signal_semas );

	//TODO
	if( vk::Result::eSuccess != graphics_queue.submit( 1, &sub_inf )){
		throw std::runtime_error( "Error during queue_submit" );
	}

	std::vector swapchains{ *device_surface->swapchains[render_info.window_index].swapchain };
	std::vector indices{ render_info.img_index };
	vk::PresentInfoKHR pres_inf(
			signal_semas,
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


	vk::RenderPassBeginInfo beg_inf(
			*mat.renderpass,
			*mat.framebuffers[render_info.img_index],
			{{ 0, 0 }, device_surface->swapchains[0].size },
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
