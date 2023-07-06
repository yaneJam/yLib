cmake_minimum_required(VERSION 3.8)
project(testLogger)



MACRO_PROJECT_ENVIRONMENT()


#set(QT_VERSION_MAJOR 6)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)


file(GLOB sources "./*.cpp" "./*.h")
add_executable(${PROJECT_NAME} ${sources})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-pthread" LINK_FLAGS "-pthread")


target_link_libraries(${PROJECT_NAME} yLib)
MACRO_PROJECT_DENPENDENCY(${PROJECT_NAME} yLib)