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

}


void SandboxLayer::onEvent( Kestrel::Event& e ){
	Kestrel::EventDispatcher d{ e };

	d.dispatch<Kestrel::KeyPushEvent>( []( Kestrel::KeyPushEvent& e ){
			KST_INFO( "Key {} pushed", e.getKeyName() );
			KST_INFO( "{}", e.scancode );
/*			// F1
			if( e.scancode == 67 ){
				Kestrel::Application::getInstance()->window[0]->setCursor( Kestrel::CursorMode::Normal );
			} else if( e.scancode == 68 ){
				Kestrel::Application::getInstance()->window[0]->setCursor( Kestrel::CursorMode::Hidden );
			} else if( e.scancode == 69 ){
				Kestrel::Application::getInstance()->window[0]->setCursor( Kestrel::CursorMode::Disabled );
			}
			*/
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

	d.dispatch<Kestrel::MouseMovedEvent>( []( Kestrel::MouseMovedEvent& e ){
			//KST_INFO( "Mouse moved to {}, {}", e.x, e.y );
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

