/*
 * =====================================================================================
 *
 *       Filename:  Log.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/24/2020 10:03:08 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include "Core/Log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Kestrel::Logger::core_logger;
std::shared_ptr<spdlog::logger> Kestrel::Logger::app_logger;


void Kestrel::Logger::LogInit(){
	spdlog::set_pattern( "[%d %m %Y %T][%n] %^%v%$" );

	core_logger = spdlog::stdout_color_mt( "KESTREL CORE" );
	app_logger = spdlog::stdout_color_mt( "KESTREL" );
}
