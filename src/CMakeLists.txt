#project name 
project(yLib) 
#cmake version
cmake_minimum_required (VERSION 3.8)

get_filename_component(PROJECT_FILE_NAME "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)


list(APPEND CMAKE_MODULE_PATH "${PROJECT_FILE_NAME}/lib/cmake")
list(APPEND CMAKE_PREFIX_PATH "${PROJECT_FILE_NAME}/lib/cmake")
include(ySetupProject)
MACRO_LOG( " PROJECT_FILE_NAME:${PROJECT_FILE_NAME}")



set(Y_BUILD_SLN CACHE STATIC INTERNAL FORCE)
set(REPOSITORY_PATH "${PROJECT_FILE_NAME})/lib/")


AddProject("${PROJECT_FILE_NAME}" yLib "test")
AddProject("${PROJECT_FILE_NAME}" testLogger "test")


MACRO_UPDATE_DENPENDENCY()


