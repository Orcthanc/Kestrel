#include <kstpch.hpp>
#include "Renderer/NaiveCamera.hpp"

#include "Core/Application.hpp"
#include "Platform/Vulkan/VKCameraRenderer.hpp"

#include <glm/gtc/reciprocal.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace Kestrel;

NaiveCamera::NaiveCamera( float fov, float aspect, float near_plane, float far_plane ): fov( fov ), near_plane( near_plane ), far_plane( far_plane ), aspect( aspect ){
	PROFILE_FUNCTION();
	
	view = glm::lookAt( glm::vec3{}, glm::vec3( 0, 0, 1 ), glm::vec3( 0.0, 1.0, 0.0 ));

	recalc_proj();

	KST_CORE_INFO("New CameraMode: {}", to_string( camera_render_mode ));

	set_renderer( std::make_unique<KST_VK_CameraRenderer>( &std::dynamic_pointer_cast<KST_VK_Context>( Application::getInstance()->graphics_context )->device ));
}

void NaiveCamera::recalc_aspect_ratio( float aspect ){
	PROFILE_FUNCTION();

	this->aspect = aspect;

	recalc_proj();
}

bool NaiveCamera::onSizeChange( WindowResizeEvent& e ){
	PROFILE_FUNCTION();

	recalc_aspect_ratio( static_cast<float>( e.width ) / static_cast<float>( e.height ));
	return false;
}

void NaiveCamera::recalc_proj(){
	PROFILE_FUNCTION();

	float A, B;
	if( any_flag( camera_render_mode & RenderModeFlags::eInverse )){
		A = -far_plane / ( near_plane - far_plane ) - 1;
		B = -far_plane * near_plane / ( near_plane - far_plane );
	} else {
		A = far_plane / ( near_plane - far_plane );
		B = far_plane * near_plane / ( near_plane - far_plane );
	}
	float temp = glm::cot( 0.5 * fov );
	proj = glm::mat4(
			temp / aspect, 0, 0, 0,
			0, -temp, 0, 0,
			0, 0, A, -1,
			0, 0, B, 0 );

	/*
	KST_CORE_INFO( "{} {}", near_plane, far_plane );
	KST_CORE_INFO( "{}", glm::to_string( proj * glm::vec4( 0, 0, -near_plane, 1 )));
	KST_CORE_INFO( "{}", glm::to_string( proj * glm::vec4( 0, 0, -far_plane, 1 )));

	auto asdf = proj * glm::vec4( 0, 0, -near_plane, 1 );
	KST_CORE_INFO( "{}", glm::to_string( glm::vec3( asdf ) / asdf.w ));
	asdf = proj * glm::vec4( 0, 0, -far_plane, 1 );
	KST_CORE_INFO( "{}", glm::to_string( glm::vec3( asdf ) / asdf.w ));
	*/
}

void NaiveCamera::updateRenderMode(RenderModeFlags new_mode){
	if( any_flag(( new_mode ^ camera_render_mode ) & RenderModeFlags::eInverse )){
		camera_render_mode = new_mode;
		recalc_proj();
	}
	if( any_flag( new_mode ^ camera_render_mode )){
		KST_CORE_INFO("New CameraMode: {}", to_string( new_mode ));
	}

	camera_render_mode = new_mode;
}
