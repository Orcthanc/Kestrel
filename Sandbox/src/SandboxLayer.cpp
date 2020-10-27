#include "SandboxLayer.hpp"

#include "Renderer/Context.hpp"

#include "Platform/Vulkan/VK_Basic_Terrain.hpp"

#include "Scene/Components.hpp"

using namespace Kestrel;

SandboxLayer::SandboxLayer( const std::string& s ): Layer{ s }{
	terrain = std::make_unique<VK_BasicTerrain>();

	auto temp = Application::getInstance()->current_scene->createEntity();
	temp.addComponent<NameComponent>( "TestName" );
}

void SandboxLayer::onUpdate(){
	static int calls = 0;
	if( !( ++calls % 1000000 )){
		KST_INFO( "{}", calls );
	}
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
}

