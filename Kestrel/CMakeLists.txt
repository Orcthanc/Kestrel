project( Kestrel )

file( GLOB_RECURSE ${PROJECT_NAME}_SOURCES "*/*.cpp" )

add_library( ${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES} )

find_package( Vulkan REQUIRED )

target_precompile_headers( ${PROJECT_NAME} PUBLIC "src/kstpch.hpp" )
target_include_directories( ${PROJECT_NAME} PUBLIC "src" glm glfw spdlog::spdlog Vulkan::Vulkan )
target_link_libraries( ${PROJECT_NAME} PUBLIC glm glfw spdlog::spdlog Vulkan::Vulkan )

if( COLOR_CONSOLE )
	target_compile_definitions( ${PROJECT_NAME} PUBLIC COLOR_CONSOLE=1 )
endif( COLOR_CONSOLE )

if( KST_PROFILING )
	target_compile_definitions( ${PROJECT_NAME} PUBLIC KST_PROFILING=1 )
endif( KST_PROFILING )

