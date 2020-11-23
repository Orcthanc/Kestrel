project( lemon )

add_executable( lemon lemon.c )
add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/lempar.c"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/lempar.c" "${CMAKE_CURRENT_BINARY_DIR}/lempar.c"
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lempar.c"
	COMMENT "Copying lempar.c" )

add_custom_target( lempar DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/lempar.c )
add_dependencies( lemon lempar )

file( GLOB_RECURSE PARSER_SOURCES "*.y" )
foreach( PARSER IN LISTS PARSER_SOURCES )
	get_filename_component( FILE_NAME ${PARSER} NAME_WE )
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.h" "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.c"
		COMMAND "${CMAKE_CURRENT_BINARY_DIR}/lemon" "${PARSER}" "-d${CMAKE_CURRENT_BINARY_DIR}"
		DEPENDS ${PARSER}
		COMMENT "Compiling parser ${PARSER}" )
	add_library( ${FILE_NAME}_PARSER STATIC "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.c" "${CMAKE_CURRENT_SOURCE_DIR}/SyntaxTree.c" )
	target_include_directories( ${FILE_NAME}_PARSER PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" )
	add_dependencies( ${FILE_NAME}_PARSER lemon )
endforeach()
