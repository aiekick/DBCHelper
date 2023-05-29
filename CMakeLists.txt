cmake_minimum_required(VERSION 3.20)

set(PROJECT DBCHelper)

enable_language(C CXX)
project(${PROJECT} CXX)

set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "")
include(cmake/3rdparty.cmake)

if(MSVC)
    add_definitions(-DMSVC)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wdeprecated-declarations -Wunused-parameter")
endif ()

add_definitions(${GLFW_DEFINITIONS})
add_definitions(-DCUSTOM_IMGUIFILEDIALOG_CONFIG="../../../src/Gui/CustomImGuiFileDialogConfig.h")
add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS)
add_definitions(-DUSE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS)

file(GLOB MAIN_SOURCES ${CMAKE_SOURCE_DIR}/main.cpp)
file(GLOB IMGUIIMPL_SOURCES 
	${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.h
	${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.h
	${CMAKE_SOURCE_DIR}/3rdparty/imgui/misc/freetype/imgui_freetype.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/imgui/misc/freetype/imgui_freetype.h)
file(GLOB_RECURSE PROJECT_SRC_RECURSE 
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.cpp
	${CMAKE_SOURCE_DIR}/src/*.hpp)
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX src FILES ${PROJECT_SRC_RECURSE})
source_group(main FILES ${MAIN_SOURCES})
source_group(src\\ImGuiImpl FILES ${IMGUIIMPL_SOURCES})

add_executable(${PROJECT}
		${MAIN_SOURCES}
		${PROJECT_SRC_RECURSE}
		${IMGUIFILEDIALOG_SOURCES}
		${IMGUIIMPL_SOURCES})

###############################################################
## will put dependencies in output dir
###############################################################

add_custom_command(TARGET ${PROJECT} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT}> $<TARGET_FILE_DIR:${PROJECT}>
  COMMAND_EXPAND_LISTS)

###############################################################
###############################################################
###############################################################

set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "${PROJECT}")

add_definitions(-DUSE_STD_FILESYSTEM)
set(CMAKE_CXX_STANDARD 17)
set_target_properties(${PROJECT} PROPERTIES CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/to_delete")

###############################################################
## will install project file and dependencies in install dir
###############################################################

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin")

function(INSTALL_PROJECT target)
  install(TARGETS ${target}
   RUNTIME DESTINATION /
   COMPONENT ${target})
endfunction()

INSTALL_PROJECT(${PROJECT})
INSTALL_PROJECT(libdbcppp)
INSTALL_PROJECT(libxmlmm)

function(INSTALL_DEPS_DLL target)
  install(FILES ${CMAKE_SOURCE_DIR}/deps/${target}
   DESTINATION /
   COMPONENT ${target})
endfunction()

INSTALL_DEPS_DLL(vcruntime140.dll)
INSTALL_DEPS_DLL(vcruntime140_1.dll)

#############################################################
### VERSION #################################################
#############################################################

file(READ ${CMAKE_SOURCE_DIR}/src/Headers/Build.h BuildIdFile)
string(REGEX MATCH "#define BuildNumber ([0-9]*)" _ ${BuildIdFile})
set(BuildNumber ${CMAKE_MATCH_1})
string(REGEX MATCH "#define MinorNumber ([0-9]*)" _ ${BuildIdFile})
set(MinorNumber ${CMAKE_MATCH_1})
string(REGEX MATCH "#define MajorNumber ([0-9]*)" _ ${BuildIdFile})
set(MajorNumber ${CMAKE_MATCH_1})
set(${PROJECT}_Version "v${MajorNumber}.${MinorNumber}.${BuildNumber}")
message("Version of ${PROJECT} to install or package : ${${PROJECT}_Version}")

################################################################################################
### PACK #######################################################################################
## https://gitlab.kitware.com/cmake/community/-/wikis/doc/cpack/Component-Install-With-CPack ###
################################################################################################

if (USE_PACKAGE_TO_NSIS)
	set(CPACK_GENERATOR "NSIS")
else()
	set(CPACK_GENERATOR "ZIP")
endif()
set(CPACK_MONOLITHIC_INSTALL 1)
set(CPACK_ZIP_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_PACKAGE_FILE_NAME "${PROJECT}_v${MajorNumber}.${MinorNumber}.${BuildNumber}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT} - DBC Helper")
set(CPACK_PACKAGE_VERSION "${MajorNumber}.${MinorNumber}.${BuildNumber}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MajorNumber}")
set(CPACK_PACKAGE_VERSION_MINOR "${MinorNumber}")
set(CPACK_PACKAGE_VERSION_PATCH "${BuildNumber}")
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "${CMAKE_PROJECT_NAME}")
SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_INSTALL_PREFIX}")
include(CPack)

###############################################################
###############################################################
###############################################################

if ("${CMAKE_EXE_LINKER_FLAGS}" STREQUAL "/machine:x64")
	set(ARCH x64)
	add_definitions(-DVERSION_X64)
else()
	set(ARCH x32)
	add_definitions(-DVERSION_X32)
endif()

include_directories(
	${CMAKE_SOURCE_DIR}/lib
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/3rdparty
	${CMAKE_SOURCE_DIR}/3rdparty/vector_dbc
	${CMAKE_SOURCE_DIR}/3rdparty/vector_dbc/src
	${VECTOR_DBC_INCLUDE_DIR}
	${PLOG_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	${GLFW_INCLUDE_DIR}
	${GLAD_INCLUDE_DIR}
	${DBCPPP_INCLUDE_DIR})

target_link_libraries(${PROJECT} PRIVATE
	${OPENGL_LIBRARIES}
	${VECTOR_DBC_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${GLFW_LIBRARIES}
	${GLAD_LIBRARIES}
	${CTOOLS_LIBRARIES}
	${IMGUI_LIBRARIES}
	${TINYXML2_LIBRARIES}
	${IMGUIFILEDIALOG_LIBRARIES}
	${IMPLOT_LIBRARIES}
	${DBCPPP_LIBRARIES})

########################################################
################# BUILD INCREMENT TOOL #################
########################################################

add_subdirectory(3rdparty/BuildInc)

file(GLOB BUILD_H_FILE ${CMAKE_SOURCE_DIR}/src/Headers/Build.h)
add_custom_command(
	TARGET ${PROJECT} PRE_BUILD
	COMMAND BuildInc 1000:10 $<$<CONFIG:Debug>:${BUILD_H_FILE}>
	DEPENDS ${PROJECT})
