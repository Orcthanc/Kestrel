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

using namespace Kestrel;

WindowCloseEvent::WindowCloseEvent(){
}

WindowCloseEvent::~WindowCloseEvent(){
}

EventType WindowCloseEvent::getType(){
	return EventType::eWindowClose;
}

EventDomain::EventDomain WindowCloseEvent::getDomain(){
	return EventDomain::eApp;
}

const char* WindowCloseEvent::getName(){
	return "WindowCloseEvent";
}
