#include <memory>
#include "Application.hpp"
#include "Log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

extern std::unique_ptr<Kestrel::Application> createApplication();

int run;

int main( int argc, char** argv ){
	Kestrel::Logger::LogInit();

	using namespace std::chrono_literals;

	std::this_thread::sleep_for( 1s );

	if( argc > 1 )
		run = atoi( argv[1] );

	auto app = createApplication();
	(*app)();
}
