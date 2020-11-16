project( imgui )

set( ${PROJECT_NAME}_SOURCES "./imgui/imgui_demo.cpp" "./imgui/imgui.cpp" "./imgui/imgui_draw.cpp" "./imgui/imgui_widgets.cpp" "./imgui/backends/imgui_impl_vulkan.cpp" "./imgui/backends/imgui_impl_glfw.cpp" )

find_package( Vulkan REQUIRED )

add_library( ${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES} )
target_include_directories( ${PROJECT_NAME} PUBLIC glfm Vulkan::Vulkan "./imgui" "./imgui/backends/" )
target_link_libraries( ${PROJECT_NAME} PUBLIC glfw Vulkan::Vulkan )
