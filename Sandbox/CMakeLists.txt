project( Sandbox )

file( GLOB_RECURSE ${PROJECT_NAME}_SOURCES "src/*.cpp" )

add_executable( ${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} )
target_link_libraries( ${PROJECT_NAME} PRIVATE Kestrel )
target_include_directories( ${PROJECT_NAME} PRIVATE Kestrel "src" )

if( SB_KST_TERRAIN )
	target_compile_definitions( ${PROJECT_NAME} PUBLIC SB_KST_TERRAIN=1 )
endif( SB_KST_TERRAIN )

copy_res_dir( "res/scenes" )
