

#需要目标路径有h文件和lib文件
find_path(yLib_INCLUDE_DIR yLib.h ${CMAKE_CURRENT_LIST_DIR}/../yLib )
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    find_library(yLib_LIBRARY NAMES yLib PATHS ${CMAKE_CURRENT_LIST_DIR}/../yLib/Debug  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	find_library(yLib_LIBRARY NAMES yLib PATHS ${CMAKE_CURRENT_LIST_DIR}/../yLib  )
endif()

if (yLib_INCLUDE_DIR AND yLib_LIBRARY)
    set(yLib_FOUND TRUE)
else()
    message(STATUS "yLib_FOUND FALSE!!!!!")
endif (yLib_INCLUDE_DIR AND yLib_LIBRARY)

