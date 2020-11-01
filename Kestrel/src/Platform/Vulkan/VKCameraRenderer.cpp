#include "Platform/Vulkan/VKCameraRenderer.hpp"

#include "Scene/Components.hpp"
#include "Platform/Vulkan/VKMaterial.hpp"

using namespace Kestrel;

KST_VK_CameraRenderer::KST_VK_CameraRenderer( std::shared_ptr<KST_VK_DeviceSurface> surface ): device_surface( std::move( surface )){}

void KST_VK_CameraRenderer::setDeviceSurface( std::shared_ptr<KST_VK_DeviceSurface> surface ){
	PROFILE_FUNCTION();

	KST_CORE_ASSERT( !device_surface, "Can not assign device surface multiple times" );

	device_surface = std::move( surface );

	vk::CommandPoolCreateInfo cmd_pl_inf(
			vk::CommandPoolCreateFlagBits::eTransient,
			device_surface->queue_families.graphics.value()
		);

	render_cmd_pool = device_surface->device->createCommandPoolUnique( cmd_pl_inf );
}

void KST_VK_CameraRenderer::begin_scene( Camera& c ){
	PROFILE_FUNCTION();
	vk::CommandBufferAllocateInfo alloc_inf(
			*render_cmd_pool,
			vk::CommandBufferLevel::ePrimary,
			1
		);

	render_info.cmd_buffer = device_surface->device->allocateCommandBuffersUnique( alloc_inf );

	vk::CommandBufferBeginInfo beg_inf( {}, {} );
	render_info.cmd_buffer[0]->begin( beg_inf );
}

void KST_VK_CameraRenderer::draw( Entity e ){
	PROFILE_FUNCTION();

	KST_CORE_ASSERT( e.hasComponent<TransformComponent>(), "Need a transform component to draw {}", e.getComponent<NameComponent>().name );
	KST_CORE_ASSERT( e.hasComponent<MeshComponent>(), "Need a mesh component to draw {}", e.getComponent<NameComponent>().name );
	KST_CORE_ASSERT( e.hasComponent<MaterialComponent>(), "Need a material component to draw {}", e.getComponent<NameComponent>().name );

	auto [transform, mesh, mat] = e.getComponent<TransformComponent, MeshComponent, MaterialComponent>();

	//TODO transform

	if( mat.mat != render_info.bound_mat ){
		if( render_info.bound_mat != -1 ){
			render_info.cmd_buffer[0]->endRenderPass();
		}
		bindMat( VK_Materials::getInstance()[ mat ]);
	}

	//TODO mesh
}

void KST_VK_CameraRenderer::endScene(){
	render_info.cmd_buffer[0]->endRenderPass();
	render_info.cmd_buffer[0]->end();
}

void KST_VK_CameraRenderer::bindMat( VK_Material_T& mat ){
	PROFILE_FUNCTION();

	vk::RenderPassBeginInfo beg_inf(
			*mat.renderpass,
			*render_info.framebuffer,
			{{ 0, 0 }, device_surface->swapchains[0].size },
			{}
		);

	render_info.cmd_buffer[0]->beginRenderPass( beg_inf, vk::SubpassContents::eInline );
	render_info.cmd_buffer[0]->bindPipeline( vk::PipelineBindPoint::eGraphics, *mat.pipeline );
}
