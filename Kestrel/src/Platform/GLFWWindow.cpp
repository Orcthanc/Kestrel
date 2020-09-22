/*
 * =====================================================================================
 *
 *       Filename:  KST_GLFWWindow.cpp
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

#include "GLFWWindow.hpp"
#include "Event/Events.hpp"

using namespace Kestrel;

static int glfwInitialized = 0;

KST_GLFWWindow::KST_GLFWWindow( WindowSettings s ): w_settings{ s }{

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

	glfwSetWindowUserPointer( window, &w_settings );

	glfwSetWindowSizeCallback( window, []( GLFWwindow* window, int width, int height ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			s->width = width;
			s->height = height;
		});

	glfwSetWindowCloseCallback( window, []( GLFWwindow* window ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			WindowCloseEvent e;
			s->callback( e );
		});

	glfwSetKeyCallback( window, []( GLFWwindow* window, int key, int scancode, int action, int mods ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			switch( action ){
				case GLFW_PRESS:
				{
					KeyPushEvent e{ scancode };
					s->callback( e );
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleaseEvent e{ scancode };
					s->callback( e );
					break;
				}
				case GLFW_REPEAT:
				{
					KST_CORE_WARN( "GLFW_REPEAT unimplemented" );
					break;
				}
			}
		});

	glfwSetCursorPosCallback( window, []( GLFWwindow* window, double x, double y ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			MouseMovedEvent e( x, y );
			s->callback( e );
		});

	glfwSetScrollCallback( window, []( GLFWwindow* window, double x, double y ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			MouseScrollEvent e( x, y );
			s->callback( e );
		});

	glfwSetMouseButtonCallback( window, []( GLFWwindow* window, int button, int action, int mods ){
			WindowSettings* s = static_cast<WindowSettings*>( glfwGetWindowUserPointer( window ));
			switch( action ){
				case GLFW_PRESS:
				{
					MousePressedEvent e( button );
					s->callback( e );
					break;
				}
				case GLFW_RELEASE:
				{
					MouseReleasedEvent e( button );
					s->callback( e );
					break;
				}
			}
		});
}

KST_GLFWWindow::~KST_GLFWWindow(){
	glfwDestroyWindow( window );
	if( --glfwInitialized == 0 ){
		glfwTerminate();
	}
}

std::pair<unsigned int, unsigned int> KST_GLFWWindow::getResolution(){
	return { w_settings.width, w_settings.height };
}

void KST_GLFWWindow::onUpdate(){
	glfwPollEvents();
}

void KST_GLFWWindow::setCallback( const EventCallback& e ){
	w_settings.callback = e;
}

void KST_GLFWWindow::setCursor( const CursorMode& cm ){
	switch( cm ){
		case CursorMode::Normal:
		{
			KST_INFO( "Normal" );
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
			break;
		}
		case CursorMode::Hidden:
		{
			KST_INFO( "Hidden" );
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
			break;
		}
		case CursorMode::Disabled:
		{
			KST_INFO( "Disabled" );
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
			break;
		}
		default:
		{
			KST_CORE_ERROR( "Invalid cursor mode {}", static_cast<std::underlying_type_t<CursorMode>>( cm ));
			break;
		}
	}
}
