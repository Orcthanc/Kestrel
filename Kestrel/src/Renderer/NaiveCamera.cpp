#include <kstpch.hpp>
#include "Renderer/NaiveCamera.hpp"

//TODO remove
#include "Core/Application.hpp"
#include "Platform/Vulkan/VKCameraRenderer.hpp"

using namespace Kestrel;

NaiveCamera::NaiveCamera( float fov, float aspect, float near, float far ){
	PROFILE_FUNCTION();

	proj = glm::perspective( fov, aspect, near, far );

	//TODO
	set_renderer( std::make_unique<KST_VK_CameraRenderer>( &std::dynamic_pointer_cast<KST_VK_Context>( Application::getInstance()->graphics_context )->device ));
}
