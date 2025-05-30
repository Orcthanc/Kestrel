cmake_minimum_required( VERSION 3.7 )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

project( Kestrel )

option( COLOR_CONSOLE "Enables colored console output" OFF )
option( KST_PROFILING "Enables performance measuring" OFF )

option( SB_KST_TERRAIN "Create the Terrain Scene" ON )

find_program(GLSLANG_VALIDATOR NAMES glslangValidator)
if(NOT GLSLANG_VALIDATOR)
	message(FATAL_ERROR "glslangValidator not found!")
endif()

function(compile_shaders)
	if(NOT ARGN)
		return()
	endif()
	foreach(SHADER ${ARGN})
		get_filename_component(FILE_NAME ${SHADER} NAME)
		get_filename_component(FILE_DIR ${SHADER} DIRECTORY)
		set(output_file ${CMAKE_BINARY_DIR}/res/${PROJECT_NAME}/${FILE_DIR}/${FILE_NAME}.spv)
		set(compiled_shaders ${compiled_shaders} ${output_file})
		set(compiled_shaders ${compiled_shaders} PARENT_SCOPE)
		#message(STATUS "${CMAKE_SOURCE_DIR}")
		add_custom_command(
				OUTPUT ${output_file}
				COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/res/${PROJECT_NAME}/${FILE_DIR}"
				#COMMAND mkdir -p shaders
				COMMAND ${GLSLANG_VALIDATOR} -w -V ${PROJECT_SOURCE_DIR}/${SHADER} -o ${output_file}
				DEPENDS ${PROJECT_SOURCE_DIR}/${SHADER}
				COMMENT "Compiling shader ${output_file}"
		)
	endforeach()
	add_custom_target(shaders ALL DEPENDS ${compiled_shaders})
	add_dependencies(${PROJECT_NAME} shaders)
endfunction()

function( copy_res_dir dir )
	file( GLOB_RECURSE files "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/*" )
	#file( GLOB_RECURSE res_files "${CMAKE_BINARY_DIR}/res/${PROJECT_NAME}/${dir}/*" )
	string( REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_BINARY_DIR}/res/${PROJECT_NAME}/" res_files "${files}" )
	add_custom_command(
		OUTPUT ${res_files}
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/${dir}" "${CMAKE_BINARY_DIR}/res/${PROJECT_NAME}/${dir}"
		DEPENDS ${files}
		COMMENT "Copying directory ${dir}"
	)
	string( REGEX REPLACE "/" "_slash_" dir_name ${dir} )
	add_custom_target( ${dir_name} ALL DEPENDS ${res_files} )
	add_dependencies( ${PROJECT_NAME} ${dir_name} )
endfunction( copy_res_dir )

function( copy_file file )
		get_filename_component(FILE_NAME ${file} NAME)
		get_filename_component(FILE_DIR ${file} DIRECTORY)
	add_custom_command(
		OUTPUT "${CMAKE_BINARY_DIR}/res/${PROJECT_NAME}/${FILE_DIR}/${FILE_NAME}"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/res/${PROJECT_NAME}/${FILE_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_DIR}/${FILE_NAME}" "${CMAKE_BINARY_DIR}/res/${PROJECT_NAME}/${FILE_DIR}/${FILE_NAME}"
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_DIR}/${FILE_NAME}
		COMMENT "Copying file ${file}"
	)
	string( REGEX REPLACE "/" "_slash_" dir_name ${file} )
	add_custom_target( ${dir_name} ALL DEPENDS "${CMAKE_BINARY_DIR}/res/${PROJECT_NAME}/${FILE_DIR}/${FILE_NAME}" )
	add_dependencies( ${PROJECT_NAME} ${dir_name} )
endfunction()

add_subdirectory( external )
add_subdirectory( Kestrel )
add_subdirectory( Sandbox )

