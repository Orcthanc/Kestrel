/*
 * =====================================================================================
 *
 *       Filename:  Events.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/26/2020 06:56:37 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include "Events.hpp"
#include <GLFW/glfw3.h>

using namespace Kestrel;

EventType WindowCloseEvent::getType(){
	return EventType::eWindowClose;
}

EventDomain WindowCloseEvent::getDomain(){
	return EventDomain::eApp;
}

const char* WindowCloseEvent::getName(){
	return "WindowCloseEvent";
}


KeyEvent::KeyEvent( int keycode ): scancode( keycode ){}

const std::string KeyEvent::getKeyName(){
	//TODO replace glfw
	const char* temp = glfwGetKeyName( GLFW_KEY_UNKNOWN, scancode );
	if( temp )
		return temp;
	return "UNKNOWN_KEY_" + std::to_string( scancode );
}


KeyPushEvent::KeyPushEvent( int keycode ): KeyEvent( keycode ){}

EventType KeyPushEvent::getType(){
	return EventType::eInputKeyPressed;
}

EventDomain KeyPushEvent::getDomain(){
	return EventDomain::eInput | EventDomain::eKeyboard;
}

const char* KeyPushEvent::getName(){
	return "KeyPushEvent";
}

KeyReleaseEvent::KeyReleaseEvent( int keycode ): KeyEvent( keycode ){}

EventType KeyReleaseEvent::getType(){
	return EventType::eInputKeyReleased;
}

EventDomain KeyReleaseEvent::getDomain(){
	return EventDomain::eInput | EventDomain::eKeyboard;
}

const char* KeyReleaseEvent::getName(){
	return "KeyReleaseEvent";
}
