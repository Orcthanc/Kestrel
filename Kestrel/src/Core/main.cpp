#include <memory>
#include "Application.hpp"
#include "Log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

extern std::unique_ptr<Kestrel::Application> createApplication();

int main( int argc, char** argv ){
	Kestrel::Logger::LogInit();

	auto app = createApplication();
	(*app)();
}
