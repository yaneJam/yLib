#project name 
project(yLib) 
#cmake version
cmake_minimum_required (VERSION 3.8)

#message(STATUS " step yLib")
#get_filename_component(YLIBROOT ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)
#list(APPEND CMAKE_MODULE_PATH "${YLIBROOT}/cmake")
#list(APPEND CMAKE_PREFIX_PATH "${YLIBROOT}/cmake")

#include(ySetupProject)
#set(REPOSITORY_PATH ${YLIBROOT})
#message(status " REPOSITORY_PATH:${REPOSITORY_PATH}")

MACRO_PROJECT_ENVIRONMENT()



file(GLOB sources "./*.cpp" "./*.h")
file(GLOB headers "${PROJECT_SOURCE_DIR}/../../inc/${PROJECT_NAME}/*.h")
file(GLOB src_dir  "./Json/*.cpp" "./Json/*.h" )
#add_executable(${PROJECT_NAME} ${sources} ${headers} ${src_dir} ) 

add_library(${PROJECT_NAME} SHARED ${sources} ${headers} ${src_dir}) 
#target_compile_definitions(${PROJECT_NAME} PRIVATE IO_EXPORTS)
#target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)
#qt_disable_unicode_defines(${PROJECT_NAME})



#set_target_properties(${PROJECT_NAME} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#cotire(${PROJECT_NAME})


target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
target_link_libraries(${PROJECT_NAME} dl)
endif()




MACRO_PROJECT_DENPENDENCY(${PROJECT_NAME} )




