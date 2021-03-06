#pragma once

#include <spdlog/spdlog.h>

namespace Kestrel {

	struct Logger {

		static std::shared_ptr<spdlog::logger> core_logger;
		static std::shared_ptr<spdlog::logger> app_logger;

		static void LogInit();

	};

}

#define KST_CORE_INFO( ... ) ::Kestrel::Logger::core_logger->info( __VA_ARGS__ )
#define KST_CORE_WARN( ... ) ::Kestrel::Logger::core_logger->warn( __VA_ARGS__ )
#define KST_CORE_ERROR( ... ) ::Kestrel::Logger::core_logger->error( __VA_ARGS__ )
#define KST_CORE_CRITICAL( ... ) ::Kestrel::Logger::core_logger->critical( __VA_ARGS__ )

#define KST_INFO( ... ) ::Kestrel::Logger::app_logger->info( __VA_ARGS__ )
#define KST_WARN( ... ) ::Kestrel::Logger::app_logger->warn( __VA_ARGS__ )
#define KST_ERROR( ... ) ::Kestrel::Logger::app_logger->error( __VA_ARGS__ )
#define KST_CRITICAL( ... ) ::Kestrel::Logger::app_logger->critical( __VA_ARGS__ )

