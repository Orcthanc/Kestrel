#include <kstpch.hpp>
#include "Renderer/NaiveCamera.hpp"

//TODO remove
#include "Core/Application.hpp"
#include "Platform/Vulkan/VKCameraRenderer.hpp"

using namespace Kestrel;

NaiveCamera::NaiveCamera( float fov, float aspect, float near, float far ){
	PROFILE_FUNCTION();

	proj = glm::perspective( fov, aspect, near, far );
	proj[1][1] *= -1;
	view = glm::lookAt( glm::vec3( 2.0, 2.0, 2.0 ), glm::vec3{}, glm::vec3( 0.0, 0.0, 1.0 ));

	set_renderer( std::make_unique<KST_VK_CameraRenderer>( &std::dynamic_pointer_cast<KST_VK_Context>( Application::getInstance()->graphics_context )->device ));
}
