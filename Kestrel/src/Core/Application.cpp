#include "Application.hpp"

#include "Platform/GLFWWindow.hpp"
#include "Platform/Vulkan/VKContext.hpp"

#include "Scene/Components.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

using namespace Kestrel;

Kestrel::Application::Application( WindowSettings ws ): running( true ), stack(){
	PROFILE_SESSION_START( "startup.json" );
	PROFILE_FUNCTION();
	if( instance )
		throw std::runtime_error( "Can not create multiple applications" );
	instance = this;

	KST_GLFW_VK_Window temp{ std::move( ws )};
	temp.setCallback( std::bind( &Kestrel::Application::onEvent, this, std::placeholders::_1 ));
	graphics_context = std::make_shared<KST_VK_Context>();
	graphics_context->registerWindow( std::move( temp ));
	graphics_context->Init({ "Sandbox", 0, 0, 1 });
}

Kestrel::Application::~Application(){
	KST_CORE_INFO( "Shut down" );

	PROFILE_SESSION_END();
}

void Kestrel::Application::onEvent( Event& e ){
	PROFILE_FUNCTION();

	EventDispatcher ed( e );
	ed.dispatch<WindowCloseEvent>( std::bind( &Application::onClose, this, std::placeholders::_1 ));

	for( auto it = stack.rbegin(); it != stack.rend(); ++it ){
		(*it)->onEvent( e );
		if( e.handled )
			break;
	}
}

void Kestrel::Application::operator()(){
	PROFILE_FUNCTION();

	while( running ){

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		current_scene->onUpdate();

		for( auto l: stack ){
			l->onUpdate();
		}

		//TODO
		if( current_scene ){
			auto view = current_scene->getView<CameraComponent>();
			for( auto& c: view ){
				auto& cam = view.get( c ).camera;
				cam->begin_scene( 0 );
				//TODO
				for( auto& e: current_scene->getView<MeshComponent>() )
					cam->draw( current_scene->toEntity( e ));
				for( auto& e: current_scene->getView<TerrainComponent>() )
					cam->draw( current_scene->toEntity( e ));
				cam->endScene();
			}
		}

		for( auto l: stack ){
			l->onGui();
		}

		graphics_context->onUpdate();
	}
}

bool Kestrel::Application::onClose( WindowCloseEvent& e ){
	running = false;
	return true;
}

void Application::setCursorMode( CursorMode mode ){
	graphics_context->setCursorMode( mode );
}

int Application::getKeyState( int key ){
	return graphics_context->getKeyState( key );
}

Kestrel::Application* Kestrel::Application::instance = nullptr;
