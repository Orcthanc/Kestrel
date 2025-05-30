set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory( glfw )
add_subdirectory( json )
add_subdirectory( imgui )
add_subdirectory( spdlog )

add_library( entt INTERFACE )
target_include_directories( entt INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/entt/include" )
