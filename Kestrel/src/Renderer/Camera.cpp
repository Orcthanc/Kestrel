#include "Renderer/Camera.hpp"

using namespace Kestrel;

void Camera::move( float x, float y, float z ){
	PROFILE_FUNCTION();

	pos += glm::vec3{ x, y, z };
	recalc_view();
}

void Camera::rotate( float x, float y, float z, float angle ){
	PROFILE_FUNCTION();

	rot *= glm::normalize( glm::angleAxis( angle, glm::vec3{ x, y, z }));
	recalc_view();
}

void Camera::rotate( const glm::quat& quaternion ){
	PROFILE_FUNCTION();

	rot *= quaternion;
	recalc_view();
}

void Camera::recalc_view(){
	PROFILE_FUNCTION();

	view = glm::translate( glm::mat4_cast( rot ), -pos );
}
