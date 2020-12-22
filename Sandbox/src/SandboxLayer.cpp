#include "SandboxLayer.hpp"

#include "Renderer/Context.hpp"

#include "Platform/Vulkan/VKMaterial.hpp"
#include "Platform/Vulkan/VKMesh.hpp"

#include "Scene/Components.hpp"
#include "Renderer/CameraModes.hpp"

#include "glm/gtc/quaternion.hpp"
#include "imgui.h"

using namespace Kestrel;

SandboxLayer::SandboxLayer( const std::string& s ): Layer{ s }{
	//terrain = std::make_unique<VK_BasicTerrain>();
/*
	auto temp = Application::getInstance()->current_scene->createEntity( "TestName" );
	auto mat = VK_Materials::getInstance().loadMaterial( "../res/Kestrel/shader/basic" );
	auto mesh = std::make_shared<Mesh>();
	mesh->load_obj<VK_Mesh>( "../res/Kestrel/res/models/Terrain4x4.obj" );
	//mesh->load_obj<VK_Mesh>( "../res/Kestrel/res/models/Test.obj" );
	temp.addComponent<MeshComponent>( mesh );
	temp.addComponent<MaterialComponent>( mat );
	temp.addComponent<TransformComponent>( glm::vec3{ 0, 0, 0.0 }, glm::quat( glm::vec3{ 0.0f, 0.0f, 0.2f }), glm::vec3{ 0.9, 0.9, 1.0 });
*/

	auto cam = Application::getInstance()->current_scene->createEntity( "Camera" );
	cam.addComponent<TransformComponent>();
	camera = std::make_shared<NaiveCamera>( 45.0, 960.0/1080.0, 0.1, 100000000.0 );
	cam.addComponent<CameraComponent>( camera );
}

void SandboxLayer::onUpdate(){
	float x = 0, y = 0, z = 0;
	Application& temp = *Application::getInstance();

	//TODO move to onEvent
	if( temp.getKeyState( GLFW_KEY_W ) == GLFW_PRESS ){
		z -= 0.1;
	}
	if( temp.getKeyState( GLFW_KEY_S ) == GLFW_PRESS ){
		z += 0.1;
	}
	if( temp.getKeyState( GLFW_KEY_A ) == GLFW_PRESS ){
		x -= 0.1;
	}
	if( temp.getKeyState( GLFW_KEY_D ) == GLFW_PRESS ){
		x += 0.1;
	}
	if( temp.getKeyState( GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS ){
		y -= 0.1;
	}
	if( temp.getKeyState( GLFW_KEY_SPACE ) == GLFW_PRESS ){
		y += 0.1;
	}
	if( x || y || z )
		camera->move( x, y, z );
}


void SandboxLayer::onEvent( Kestrel::Event& e ){
	Kestrel::EventDispatcher d{ e };

	d.dispatch<Kestrel::KeyPushEvent>( []( Kestrel::KeyPushEvent& e ){
			KST_INFO( "Key {} pushed", e.getKeyName() );
			KST_INFO( "{}", e.scancode );
			// F1
			static bool cursor_visible = true;
			if( e.scancode == 67 ){
				if( cursor_visible )
					Kestrel::Application::getInstance()->setCursorMode( Kestrel::CursorMode::Disabled );
				else
					Kestrel::Application::getInstance()->setCursorMode( Kestrel::CursorMode::Normal );
				cursor_visible = !cursor_visible;
			}
			
			return true;
		});

	d.dispatch<Kestrel::KeyReleaseEvent>( []( Kestrel::KeyReleaseEvent& e ){
			KST_INFO( "Key {} released", e.getKeyName() );
			return true;
		});

	d.dispatch<Kestrel::MousePressedEvent>( []( Kestrel::MousePressedEvent& e ){
			KST_INFO( "Key {} pressed", e.getButtonName() );
			return true;
		});

	d.dispatch<Kestrel::MouseReleasedEvent>( []( Kestrel::MouseReleasedEvent& e ){
			KST_INFO( "Key {} released", e.getButtonName() );
			return true;
		});

	d.dispatch<Kestrel::MouseMovedEvent>( [&]( Kestrel::MouseMovedEvent& e ){
			//KST_INFO( "Mouse moved to {}, {}", e.x, e.y );
			const double rot_speed = 0.01;
			static double last_x = e.x;
			static double last_y = e.y;
			camera->rotate_global( glm::quat( glm::vec3{ 0, ( e.x - last_x ) * rot_speed, 0 }));
			camera->rotate( glm::quat( glm::vec3{ ( e.y - last_y ) * rot_speed, 0, 0 }));
			last_x = e.x;
			last_y = e.y;
			return true;
		});

	d.dispatch<Kestrel::MouseScrollEvent>( []( Kestrel::MouseScrollEvent& e ){
			KST_INFO( "Mouse scrolled {}, {}", e.x, e.y );
			return true;
		});

	d.dispatch<Kestrel::WindowResizeEvent>( []( Kestrel::WindowResizeEvent& e ){
			auto cams = Application::getInstance()->current_scene->getView<CameraComponent>();
			for( auto& i: cams ){
				auto cam = cams.get( i );

				cam.camera->onSizeChange( e );
			}

			return false;
		});
}

