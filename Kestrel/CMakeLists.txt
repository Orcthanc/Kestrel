project( Kestrel )

file( GLOB_RECURSE ${PROJECT_NAME}_SOURCES "*/*.cpp" )

add_library( ${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES} )

target_precompile_headers( ${PROJECT_NAME} PUBLIC "src/kstpch.hpp" )
target_include_directories( ${PROJECT_NAME} PUBLIC "src" glm glfw spdlog::spdlog )
target_link_libraries( ${PROJECT_NAME} PUBLIC glm glfw spdlog::spdlog )
