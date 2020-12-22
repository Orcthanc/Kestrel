#pragma once

#include <kstpch.hpp>

#include "Core/LayerStack.hpp"
#include "Core/Window.hpp"
#include "Core/Input.hpp"

#include "Event/Events.hpp"
#include "Renderer/Context.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"

#include "GLFW/glfw3.h"

int main( int, char** );

namespace Kestrel {
	class Application {
		public:
			Application( WindowSettings w = {} );
			virtual ~Application();

			void addLayer( std::shared_ptr<Layer> l ){ stack.pushLayer( l ); };
			void removeLayer( std::shared_ptr<Layer> l ){ stack.PopLayer( l ); };
			void onEvent( Event& e );
			bool onClose( WindowCloseEvent& e );

			void setCursorMode( CursorMode mode );
			int getKeyState( int key );

			static Application* getInstance(){ return instance; }

			bool running;

			//TODO keep this, or is the scene owned by a layer, or the layers owned by the scene?
			std::shared_ptr<Scene> current_scene;
			std::shared_ptr<Context> graphics_context;
		private:
			LayerStack stack;

			void operator()();
			friend int ::main( int, char** );


			static Application* instance;
	};
}
