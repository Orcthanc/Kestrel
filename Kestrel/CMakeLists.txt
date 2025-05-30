project( Kestrel )

file( GLOB_RECURSE ${PROJECT_NAME}_SOURCES "*/*.cpp" )

add_library( ${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES} )

find_package( glm REQUIRED )
find_package( Vulkan REQUIRED )

target_precompile_headers( ${PROJECT_NAME} PUBLIC "src/kstpch.hpp" )
target_include_directories( ${PROJECT_NAME} PUBLIC "src" )
target_link_libraries( ${PROJECT_NAME} PUBLIC glm::glm glfw spdlog::spdlog Vulkan::Vulkan entt imgui nlohmann_json::nlohmann_json )

if( COLOR_CONSOLE )
	target_compile_definitions( ${PROJECT_NAME} PUBLIC COLOR_CONSOLE=1 )
endif( COLOR_CONSOLE )

if( KST_PROFILING )
	target_compile_definitions( ${PROJECT_NAME} PUBLIC KST_PROFILING=1 )
endif( KST_PROFILING )

copy_file( "shader/basic.json" )
copy_file( "shader/terrain.json" )

compile_shaders(
	"shader/basic.vert"
	"shader/basic.frag"
	"shader/basic.tesc"
	"shader/basic.tese"
	"shader/terrain.vert"
	"shader/terrain.frag"
	"shader/terrain.tesc"
	"shader/terrain.tese"
	)

copy_res_dir( "res/models" )
