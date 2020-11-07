#include "Application.hpp"

#include "Platform/GLFWWindow.hpp"
#include "Platform/Vulkan/VKContext.hpp"

#include "Scene/Components.hpp"

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

	current_scene = std::make_shared<Scene>();
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
				for( auto& e: current_scene->getView<MeshComponent>() )
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

Kestrel::Application* Kestrel::Application::instance = nullptr;
