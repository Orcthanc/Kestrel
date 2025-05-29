#include "Renderer/Camera.hpp"

using namespace Kestrel;

void Camera::move( float x, float y, float z ){
	PROFILE_FUNCTION();

	pos += glm::vec3{ x, y, z } * rot;
	recalc_view();
}

void Camera::rotate( float x, float y, float z, float angle ){
	PROFILE_FUNCTION();

	KST_CORE_WARN( "Camera::rotate(float, float, float, float) is probably buggy" );

	rot *= glm::normalize( glm::angleAxis( angle, glm::vec3{ x, y, z }));
	recalc_view();
}

void Camera::rotate( const glm::quat& quaternion ){
	PROFILE_FUNCTION();

	rot = quaternion * rot;
	recalc_view();
}

void Camera::rotate_global( const glm::quat& quaternion ){
	PROFILE_FUNCTION();

	rot = rot * quaternion;
	recalc_view();
}

void Camera::onImgui(){}

void Camera::recalc_view(){
	PROFILE_FUNCTION();

	view = glm::mat4_cast( rot ) * glm::translate( glm::identity<glm::mat4>(), -pos );
	//KST_CORE_INFO( "\n{}\n{}\n{}", glm::to_string( pos ), glm::to_string( view ), glm::to_string( view ));
}

bool Camera::onSizeChange(Kestrel::WindowResizeEvent &e){
	PROFILE_FUNCTION();
	return false;
}

void Camera::set_renderer( std::unique_ptr<CameraRenderer>&& r ){
	renderer = std::move( r );
}

void Camera::begin_scene( size_t window_index ){
	renderer->begin_scene( *this, window_index );
}

void Camera::draw( Entity e ){
	renderer->draw( e );
}

void Camera::endScene(){
	renderer->endScene();
}

void Camera::updateRenderMode(RenderModeFlags new_mode){
	camera_render_mode = new_mode;
}
