#include <kstpch.hpp>
#include "Renderer/NaiveCamera.hpp"

#include "Core/Application.hpp"
#include "Platform/Vulkan/VKCameraRenderer.hpp"

using namespace Kestrel;

NaiveCamera::NaiveCamera( float fov, float aspect, float near_plane, float far_plane ): fov( fov ), near_plane( near_plane ), far_plane( far_plane ){
	PROFILE_FUNCTION();

	proj = glm::perspectiveZO( fov, aspect, near_plane, far_plane );
	proj[1][1] *= -1;
	view = glm::lookAt( glm::vec3{}, glm::vec3( 0, 0, 1 ), glm::vec3( 0.0, 1.0, 0.0 ));

	camera_render_mode = RenderModeFlags::eNone;

	set_renderer( std::make_unique<KST_VK_CameraRenderer>( &std::dynamic_pointer_cast<KST_VK_Context>( Application::getInstance()->graphics_context )->device ));
}

void NaiveCamera::recalc_aspect_ratio( float aspect ){
	PROFILE_FUNCTION();

	proj = glm::perspectiveZO( fov, aspect, near_plane, far_plane );
	proj[1][1] *= -1;
}

bool NaiveCamera::onSizeChange( WindowResizeEvent& e ){
	PROFILE_FUNCTION();

	recalc_aspect_ratio( static_cast<float>( e.width ) / static_cast<float>( e.height ));
	return false;
}
