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
#include "Sandbox.h"

Kestrel::Application* createApplication(){
	return new Sandbox();
}
