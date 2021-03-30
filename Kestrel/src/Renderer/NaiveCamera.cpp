#include <kstpch.hpp>
#include "Renderer/NaiveCamera.hpp"

#include "Core/Application.hpp"
#include "Platform/Vulkan/VKCameraRenderer.hpp"

#include <glm/gtc/reciprocal.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace Kestrel;

NaiveCamera::NaiveCamera( float fov, float aspect, float near_plane, float far_plane ): fov( fov ), near_plane( near_plane ), far_plane( far_plane ), aspect( aspect ){
	PROFILE_FUNCTION();

	recalc_view();
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
#if 1
		A = -far_plane / ( near_plane - far_plane ) - 1;
		B = -far_plane * near_plane / ( near_plane - far_plane );
#else
		A = 0;
		B = near_plane;
#endif
	} else {
		A = far_plane / ( near_plane - far_plane );
		B = far_plane * near_plane / ( near_plane - far_plane );
	}
	float temp = static_cast<float>( glm::cot( 0.5 * fov ));
	proj = glm::mat4(
			temp / aspect, 0, 0, 0,
			0, -temp, 0, 0,
			0, 0, A, -1,
			0, 0, B, 0 );
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

void NaiveCamera::onImgui(){
	bool inverse     = any_flag( camera_render_mode & RenderModeFlags::eInverse );
	bool logarithmic = any_flag( camera_render_mode & RenderModeFlags::eLogarithmic );
	bool integer     = any_flag( camera_render_mode & RenderModeFlags::eIntegerDepth );
	bool wireframe   = any_flag( camera_render_mode & RenderModeFlags::eWireframe );

	ImGui::Checkbox( "Inverse depth buffer", &inverse );
	ImGui::Checkbox( "Logarithmic depth buffer", &logarithmic );
	ImGui::Checkbox( "Integer depth buffer (unimplemented)", &integer );
	ImGui::Checkbox( "Wireframe", &wireframe );

	RenderModeFlags res{ RenderModeFlags::eNone };

	res |= inverse     ? RenderModeFlags::eInverse      : RenderModeFlags::eNone;
	res |= logarithmic ? RenderModeFlags::eLogarithmic  : RenderModeFlags::eNone;
	res |= integer     ? RenderModeFlags::eIntegerDepth : RenderModeFlags::eNone;
	res |= wireframe   ? RenderModeFlags::eWireframe    : RenderModeFlags::eNone;

	updateRenderMode( res );
}
