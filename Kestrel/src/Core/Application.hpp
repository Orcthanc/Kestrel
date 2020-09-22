/*
 * =====================================================================================
 *
 *       Filename:  Application.h
 *
 *    Description:  Main application to inherit
 *
 *        Version:  1.0
 *        Created:  08/23/2020 03:06:43 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#pragma once

#include "kstpch.hpp"
#include "LayerStack.hpp"
#include "Window.hpp"
#include "Event/Events.hpp"

int main( int, char** );

namespace Kestrel {
	class Application {
		public:
			Application( WindowSettings w = {} );
			virtual ~Application() = default;

			void addLayer( Layer* l ){ stack.pushLayer( l ); };
			void removeLayer( Layer* l ){ stack.PopLayer( l ); };
			void onEvent( Event& e );
			bool onClose( WindowCloseEvent& e );

			static Application* getInstance(){ return instance; }


			bool running;

			std::unique_ptr<Window> window;
		private:
			LayerStack stack;

			void operator()();
			friend int ::main( int, char** );


			static Application* instance;
	};
}
