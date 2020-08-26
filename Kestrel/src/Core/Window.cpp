/*
 * =====================================================================================
 *
 *       Filename:  Window.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/26/2020 02:27:14 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include "Window.hpp"

using namespace Kestrel;

static int glfwInitialized = 0;

Window::Window( WindowSettings s ): s{ s }{

	if( !glfwInitialized++ ){
		if( !glfwInit() ){
			KST_CORE_CRITICAL( "Could not create glfw context" );
			throw std::runtime_error( "Could not create glfw context" );
		}

		glfwSetErrorCallback( []( int error, const char* description ){ KST_CORE_ERROR( "GLFW Error: {} ({})" ); });
	}

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	window = glfwCreateWindow( s.width, s.height, s.name.c_str(), NULL, NULL );

	if( !window ){
		KST_CORE_CRITICAL( "Could not create glfw window" );
		throw std::runtime_error( "Could not create glfw window" );
	}

	glfwSetWindowUserPointer( window, &s );

	glfwSetWindowSizeCallback( window, []( GLFWwindow* window, int width, int height ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			s->width = width;
			s->height = height;
		});

	glfwSetWindowCloseCallback( window, []( GLFWwindow* window ){
			KST_CORE_ASSERT( false, "TODO::fixme::Unimplemented method" );
		});

	glfwSetKeyCallback( window, []( GLFWwindow* window, int key, int scancode, int action, int mods ){
			switch( action ){
				case GLFW_PRESS:
				{
					KST_CORE_WARN( "GLFW_PRESS unimplemented" );
				}
				break;
				case GLFW_RELEASE:
				{
					KST_CORE_WARN( "GLFW_RELEASE unimplemented" );
				}
				break;
				case GLFW_REPEAT:
				{
					KST_CORE_WARN( "GLFW_REPEAT unimplemented" );
				}
				break;
			}
		});

	glfwSetCursorPosCallback( window, []( GLFWwindow* window, double x, double y ){
			KST_CORE_INFO( "Mouse moved to {}, {}", x, y );
			KST_CORE_WARN( "CursorPosCallback unimplemented" );
		});

	glfwSetScrollCallback( window, []( GLFWwindow* window, double x, double y ){
			KST_CORE_WARN( "ScrollCallback unimplemented" );
		});

	glfwSetMouseButtonCallback( window, []( GLFWwindow* window, int button, int action, int mods ){
			KST_CORE_WARN( "MouseButtonCallback unimplemented" );
		});
}

Window::~Window(){
	glfwDestroyWindow( window );
	if( --glfwInitialized == 0 ){
		glfwTerminate();
	}
}

std::pair<unsigned int, unsigned int> Window::getResolution(){
	return { s.width, s.height };
}

void Window::onUpdate(){
	glfwPollEvents();
}

void Window::setCallback( EventCallback e ){
	callback = e;
}
