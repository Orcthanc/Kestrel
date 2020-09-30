/*
 * =====================================================================================
 *
 *       Filename:  Application.cpp
 *
 *    Description:  Implementation of Application.h
 *
 *        Version:  1.0
 *        Created:  08/23/2020 03:21:41 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include "Application.hpp"

#include "Platform/GLFWWindow.hpp"
#include "Platform/Vulkan/VKContext.hpp"

Kestrel::Application::Application( WindowSettings w ): running( true ), stack(){
	PROFILE_SESSION_START( "startup.json" );
	PROFILE_FUNCTION();
	window = std::make_unique<KST_GLFWWindow>( std::move( w ));
	if( instance )
		throw std::runtime_error( "Can not create multiple applications" );
	instance = this;
	window->setCallback( std::bind( &Kestrel::Application::onEvent, this, std::placeholders::_1 ));
	graphics_context = std::make_unique<KSTVKContext>();
	graphics_context->Init({ "Sandbox", 0, 0, 1 });
}

Kestrel::Application::~Application(){
	KST_CORE_INFO( "Shut down" );
	PROFILE_SESSION_END();
}

void Kestrel::Application::onEvent( Event& e ){
	EventDispatcher ed( e );
	ed.dispatch<WindowCloseEvent>( std::bind( &Application::onClose, this, std::placeholders::_1 ));

	for( auto it = stack.rbegin(); it != stack.rend(); ++it ){
		(*it)->onEvent( e );
		if( e.handled )
			break;
	}
}

void Kestrel::Application::operator()(){
	while( running ){
		window->onUpdate();

		for( auto l: stack ){
			l->onUpdate();
		}

		for( auto l: stack ){
			l->onGui();
		}
	}
}

bool Kestrel::Application::onClose( WindowCloseEvent& e ){
	running = false;
	return true;
}

Kestrel::Application* Kestrel::Application::instance = nullptr;
