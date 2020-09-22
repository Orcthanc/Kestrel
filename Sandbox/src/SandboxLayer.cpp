/*
 * =====================================================================================
 *
 *       Filename:  SandboxLayer.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/25/2020 05:14:28 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include "SandboxLayer.hpp"

SandboxLayer::SandboxLayer( const std::string& s ): Layer{ s }{}

void SandboxLayer::onUpdate(){
	static int calls = 0;
	if( !( ++calls % 1000000 ))
		KST_INFO( "{}", calls );
}


void SandboxLayer::onEvent( Kestrel::Event& e ){
	Kestrel::EventDispatcher d{ e };

	d.dispatch<Kestrel::KeyPushEvent>( []( Kestrel::KeyPushEvent& e ){
			KST_INFO( "Key {} pushed", e.getKeyName() );
			KST_INFO( "{}", e.scancode );
			// F1
			if( e.scancode == 67 ){
				Kestrel::Application::getInstance()->window->setCursor( Kestrel::CursorMode::Normal );
			} else if( e.scancode == 68 ){
				Kestrel::Application::getInstance()->window->setCursor( Kestrel::CursorMode::Hidden );
			} else if( e.scancode == 69 ){
				Kestrel::Application::getInstance()->window->setCursor( Kestrel::CursorMode::Disabled );
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

	d.dispatch<Kestrel::MouseMovedEvent>( []( Kestrel::MouseMovedEvent& e ){
			KST_INFO( "Mouse moved to {}, {}", e.x, e.y );
			return true;
		});

	d.dispatch<Kestrel::MouseScrollEvent>( []( Kestrel::MouseScrollEvent& e ){
			KST_INFO( "Mouse scrolled {}, {}", e.x, e.y );
			return true;
		});
}

