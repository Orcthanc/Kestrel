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

Kestrel::Application::Application( WindowSettings w ): running( true ), stack(), window{ w }{
	if( instance )
		throw std::runtime_error( "Can not create multiple applications" );
	instance = this;
}

void Kestrel::Application::operator()(){
	while( running ){
		window.onUpdate();

		for( auto l: stack ){
			l->onUpdate();
		}

		for( auto l: stack ){
			l->onGui();
		}
	}
}

Kestrel::Application* Kestrel::Application::instance = nullptr;
