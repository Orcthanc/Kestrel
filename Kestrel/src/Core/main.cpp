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
#include "Log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

extern Kestrel::Application* createApplication();

int main( int argc, char** argv ){
	Kestrel::Logger::LogInit();

	KST_CORE_WARN( "Warn" );
	KST_CORE_INFO( "Test {} {} {}", 1, 2, 3 );
	KST_ERROR( "Error" );
	KST_CRITICAL( "Critical" );

	auto app = std::unique_ptr<Kestrel::Application>( createApplication() );
	(*app)();
}
