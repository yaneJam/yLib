MACRO(MACRO_LOG )
message("<${CMAKE_CURRENT_LIST_FILE}:${CMAKE_CURRENT_LIST_LINE}>  ${ARGV0}")
ENDMACRO(MACRO_LOG)

MACRO(AddProject)
set(PROJECT_DIR ${ARGV0})
set(PROJECT_NAME ${ARGV1})
if(PARAM_CODE_TYPE STREQUAL "ALL_LOCAL")
    if (EXISTS "${PROJECT_FILE_NAME}/src/${PROJECT_NAME}/CMakeLists.txt") 
		add_subdirectory(${PROJECT_FILE_NAME}/src/${PROJECT_NAME}  ${PROJECT_NAME})
		if(${ARGC} GREATER 2)
			set(GROUP ${ARGV2})
			SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${GROUP})
		endif()
	else()
		message(STATUS "no ${PROJECT_NAME}")
	endif()
	
elseif (EXISTS "${PROJECT_DIR}/src/${PROJECT_NAME}/CMakeLists.txt") 
	add_subdirectory(${PROJECT_DIR}/src/${PROJECT_NAME} ${PROJECT_NAME})
	if(${ARGC} GREATER 2)
		set(GROUP ${ARGV2})
		SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${GROUP})
	endif()
else()
	#message(STATUS "no ${PROJECT_NAME}")
endif ()
ENDMACRO(AddProject)


MACRO(MACRO_SETUP_PROJECT )
set(PROJECT_OUTPUT_DIR "${PROJECT_BINARY_DIR}/../../bin/${TARGET_MACHINE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/../../lib/${PROJECT_NAME}/${TARGET_MACHINE}/Debug")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/../../lib/${PROJECT_NAME}/${TARGET_MACHINE}/Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_OUTPUT_DIR}/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_OUTPUT_DIR}/Release")
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/../../lib/${PROJECT_NAME}")
include_directories("${PROJECT_BINARY_DIR}/../../lib")
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${REPOSITORY_PATH}")
link_directories("${PROJECT_BINARY_DIR}/../../lib")
link_directories("${PROJECT_BINARY_DIR}")
link_directories("${PROJECT_SOURCE_DIR}")
link_directories("${REPOSITORY_PATH}")
ENDMACRO(MACRO_SETUP_PROJECT)


MACRO(MACRO_PROJECT_ENVIRONMENT)
set(PROJECT_OUTPUT_DIR "${PROJECT_BINARY_DIR}/../../bin/${TARGET_MACHINE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/../../lib/${PROJECT_NAME}/${TARGET_MACHINE}/Debug")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/../../lib/${PROJECT_NAME}/${TARGET_MACHINE}/Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_OUTPUT_DIR}/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_OUTPUT_DIR}/Release")


set(CMAKE_BINARY_DIR "${PROJECT_BINARY_DIR}/../../bin/${TARGET_MACHINE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../../lib/${PROJECT_NAME}/${TARGET_MACHINE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../../lib/${PROJECT_NAME}/${TARGET_MACHINE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}")


include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/../../lib/${PROJECT_NAME}")
include_directories("${PROJECT_BINARY_DIR}/../../lib")
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${REPOSITORY_PATH}")
link_directories("${PROJECT_BINARY_DIR}/../../lib")
link_directories("${PROJECT_BINARY_DIR}")
link_directories("${PROJECT_SOURCE_DIR}")
link_directories("${REPOSITORY_PATH}")
ENDMACRO(MACRO_PROJECT_ENVIRONMENT)


MACRO(MACRO_PROJECT_ENVIRONMENT_NDR)
set(PROJECT_OUTPUT_DIR "${PROJECT_BINARY_DIR}/../../bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/../../lib/${PROJECT_NAME}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/../../lib/${PROJECT_NAME}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_OUTPUT_DIR}")

set(CMAKE_BINARY_DIR "${PROJECT_BINARY_DIR}/../../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../../lib/${PROJECT_NAME}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../../lib/${PROJECT_NAME}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}")

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/../../lib/${PROJECT_NAME}")
include_directories("${PROJECT_BINARY_DIR}/../../lib")
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${REPOSITORY_PATH}")
link_directories("${PROJECT_BINARY_DIR}/../../lib")
link_directories("${PROJECT_BINARY_DIR}")
link_directories("${PROJECT_SOURCE_DIR}")
link_directories("${REPOSITORY_PATH}")
ENDMACRO(MACRO_PROJECT_ENVIRONMENT)




function(MACRO_PROJECT_DENPENDENCY)
math(EXPR COUNT "${ARGC} - 1")
set(PROJECT_NAME ${ARGV0})

foreach(index RANGE 1 ${COUNT})
list(APPEND DEPENDENCIES ${ARGV${index}})
endforeach()

set(${PROJECT_NAME}_DEPENDENCIES ${DEPENDENCIES} CACHE STATIC INTERNAL FORCE )
set(LOCAL_Y_BUILD_SLN ${Y_BUILD_SLN})

list(APPEND LOCAL_Y_BUILD_SLN ${PROJECT_NAME} )
set(Y_BUILD_SLN ${LOCAL_Y_BUILD_SLN} CACHE STATIC INTERNAL FORCE )

if (EXISTS "${CMAKE_ROOT}/Modules/yAppendDependencies.cmake") 
INCLUDE(yAppendDependencies)
MACRO_UPDATE_INC()
endif()

endfunction(MACRO_PROJECT_DENPENDENCY)


MACRO(MACRO_PROJECT_DLL)
file(GLOB sources "./*.cpp" "./*.h")
file(GLOB headers "${PROJECT_SOURCE_DIR}/../../lib/${PROJECT_NAME}/*.h")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm256")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG /OPT:REF /OPT:ICF")
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
if(QTENHANCED)
file(GLOB QTMocHeaders
  ${PROJECT_SOURCE_DIR}/../../lib/${PROJECT_NAME}/*.h
  )
file(GLOB QTUicUIs
  *.ui
  )
#INCLUDE(yQt5WarpEx)
#qt5_wrap_cpp_ex(LibMocSrcs ${QTMocHeaders})
#qt5_wrap_ui(UicHeaders ${QTUicUIs})
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(${PROJECT_NAME} SHARED ${sources} ${headers} ${QTUicUIs} ${UicHeaders} ${LibMocSrcs} ${QTMocHeaders}) 

#source_group(moc FILES ${LibMocSrcs})

#INCLUDE(QtPlatformTargetHelpers)
#qt_enable_utf8_sources(${PROJECT_NAME})

qt_disable_unicode_defines(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Widgets)	
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Sql)		
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core5Compat)	
else()
add_library(${PROJECT_NAME} SHARED ${sources} ${headers}) 
endif(QTENHANCED)
target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
endif ()
ENDMACRO(MACRO_PROJECT_DLL)



MACRO(Y_PROJECT_DLL)
file(GLOB sources "./*.cpp" "./*.h")
file(GLOB headers "${PROJECT_SOURCE_DIR}/../../lib/${PROJECT_NAME}/*.h")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm256")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG /OPT:REF /OPT:ICF")
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
if(QTENHANCED)
file(GLOB QTMocHeaders
  ${PROJECT_SOURCE_DIR}/../../lib/${PROJECT_NAME}/*.h
  )
file(GLOB QTUicUIs
  *.ui
  )
INCLUDE(yQt5WarpEx)
qt5_wrap_cpp_ex(LibMocSrcs ${QTMocHeaders})
qt5_wrap_ui(UicHeaders ${QTUicUIs})
add_library(${PROJECT_NAME} SHARED ${src_dir} ${sources} ${headers} ${QTUicUIs} ${UicHeaders} ${LibMocSrcs} ${QTMocHeaders}) 
source_group(moc FILES ${LibMocSrcs})
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Widgets)	
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Sql)	
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::PrintSupport)	
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Network)	
else()
add_library(${PROJECT_NAME} SHARED ${sources} ${headers} ${src_dir}) 
endif(QTENHANCED)
target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
endif ()
ENDMACRO(Y_PROJECT_DLL)





MACRO(Y_PROJECT_EXE)
file(GLOB sources "./*.cpp" "./*.h")
file(GLOB headers "${PROJECT_SOURCE_DIR}/../../lib/${PROJECT_NAME}/*.h")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm256")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
if(QTENHANCED)
file(GLOB QTMocHeaders
  ${PROJECT_SOURCE_DIR}/../../lib/${PROJECT_NAME}/*.h
  )
file(GLOB QTUicUIs
  *.ui
  )
#INCLUDE(yQt5WarpEx)
#qt5_wrap_cpp_ex(LibMocSrcs ${QTMocHeaders})
#qt5_wrap_ui(UicHeaders ${QTUicUIs})
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
add_executable(${PROJECT_NAME} ${src_dir} ${sources} ${headers} ${QTUicUIs} ${UicHeaders} ${LibMocSrcs} ${QTMocHeaders}) 
#source_group(moc FILES ${LibMocSrcs})
qt_disable_unicode_defines(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Widgets)	
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Sql)	
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Network)	
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core5Compat)	
else()
add_executable(${PROJECT_NAME} ${sources} ${headers} ${src_dir}) 
endif(QTENHANCED)
target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND "${PROJECT_BINARY_DIR}/../../tools/update_remote_bin_dir.exe" ARGS $(RemoteDir) $(TargetDir))
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${Qt5_PATH}/bin;$(PATH)$(LocalDebuggerEnvironment) " )
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)" )

endif ()
ENDMACRO(Y_PROJECT_EXE)




function(MACRO_UPDATE_DENPENDENCY)
if (EXISTS "${CMAKE_ROOT}/Modules/yAppendDependencies.cmake") 
	INCLUDE(yAppendDependencies)
	message(STATUS "Y_BUILD_SLN:${Y_BUILD_SLN}")
	foreach(project_and_dependencies  ${Y_BUILD_SLN})
	message(STATUS "${project_and_dependencies} depends on:[${${project_and_dependencies}_DEPENDENCIES}]")
		foreach(dependency  ${${project_and_dependencies}_DEPENDENCIES})
		append_dependencies(${project_and_dependencies} ${dependency})
		endforeach()
	endforeach()
else()
	message(STATUS "MACRO_UPDATE_DENPENDENCY err")
endif ()
endfunction(MACRO_UPDATE_DENPENDENCY)


