#include <kstpch.hpp>
#include "Renderer/NaiveCamera.hpp"

using namespace Kestrel;

NaiveCamera::NaiveCamera( float fov, float aspect, float near, float far ){
	PROFILE_FUNCTION();

	proj = glm::perspective( fov, aspect, near, far );
}
