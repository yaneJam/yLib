#function(append_dependencies)
#    set(multiValueArgs DEPENDENCIES)
#    cmake_parse_arguments(APPEND_DEPENDENCIES "" "" "${multiValueArgs}" ${ARGN})
#    
#    get_target_property(OUTPUT_VALUE all STATUS)
#    if(${OUTPUT_VALUE} STREQUAL OUTPUT_VALUE-NOTFOUND)
#        add_custom_target(all DEPENDS ${APPEND_DEPENDENCIES_DEPENDENCIES})
#        set_property(TARGET all PROPERTY STATUS AVAILABLE)
#    else()
#        add_dependencies(all DEPENDS ${APPEND_DEPENDENCIES_DEPENDENCIES})
#    endif()
#    
#endfunction()

function(append_dependencies arg)
	
list(FIND  Y_BUILD_SLN ${ARGV1} found_or_not)
	
	if(found_or_not EQUAL -1)
		#message(STATUS "${ARGV1} not found ${found_or_not} !!!!!!!!!!!!!!")
	else()
		#message(STATUS "${ARGV1} found ${found_or_not} ")
		add_dependencies(${ARGV0} ${ARGV1})

	endif()
  
endfunction()

MACRO(MACRO_UPDATE_INC )
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
COMMAND "${PROJECT_BINARY_DIR}/../../tools/update_lib.sh"
ARGS ${PROJECT_SOURCE_DIR}/ ${PROJECT_NAME} "${PROJECT_BINARY_DIR}/../" ${PROJECT_FILE_NAME} 
)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
if(PARAM_CODE_TYPE STREQUAL "ALL_LOCAL")
elseif(PARAM_CODE_TYPE STREQUAL "ALL_REMOTE")
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
COMMAND "${PROJECT_BINARY_DIR}/../../tools/update_inc_src.bat"
ARGS ${PROJECT_SOURCE_DIR}/ ${PROJECT_NAME} "${PROJECT_BINARY_DIR}/../" ${PROJECT_FILE_NAME} 
)
else()
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
COMMAND "${PROJECT_BINARY_DIR}/../../tools/update_lib.sh"
ARGS ${PROJECT_SOURCE_DIR}/ ${PROJECT_NAME} "${PROJECT_BINARY_DIR}/../" ${PROJECT_FILE_NAME} 
)
	endif()
endif()
ENDMACRO(MACRO_UPDATE_INC)


