/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/23/2020 03:23:44 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */
#include <memory>
#include "Application.h"

extern Kestrel::Application* createApplication();

int main( int argc, char** argv ){
	auto app = std::unique_ptr<Kestrel::Application>( createApplication() );
	(*app)();
}
