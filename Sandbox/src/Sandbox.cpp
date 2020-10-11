/*
 * =====================================================================================
 *
 *       Filename:  Sandbox.cpp
 *
 *    Description:  Implementation of Sandbox.h
 *
 *        Version:  1.0
 *        Created:  08/23/2020 03:33:05 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */
#include "Sandbox.hpp"
#include "SandboxLayer.hpp"

Sandbox::Sandbox(): Kestrel::Application(){
	//TODO lifetime management
	addLayer( new SandboxLayer( "Sandbox" ));
}

Kestrel::Application* createApplication(){
	return new Sandbox();
}
