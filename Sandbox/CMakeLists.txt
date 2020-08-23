project( Sandbox )

file( GLOB_RECURSE ${PROJECT_NAME}_SOURCES "src/*.cpp" )

add_executable( ${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} )
target_link_libraries( ${PROJECT_NAME} PRIVATE Kestrel )
target_include_directories( ${PROJECT_NAME} PRIVATE Kestrel )
