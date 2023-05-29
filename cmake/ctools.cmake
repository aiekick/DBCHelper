set(USE_GL_VERSION_CHECKER ON CACHE BOOL "")
set(USE_CONFIG_SYSTEM ON CACHE BOOL "")

if(USE_SDL)
set(USE_SDL_CLIPBOARD ON CACHE BOOL "")
set(USE_GLFW_CLIPBOARD OFF CACHE BOOL "")
else()
set(USE_GLFW_CLIPBOARD ON CACHE BOOL "")
set(USE_SDL_CLIPBOARD OFF CACHE BOOL "")
endif()

include_directories(${GLFW_INCLUDE_DIR})

add_definitions(-DUSE_OPENGL)
add_definitions(-DUSE_IMGUI="../imgui/imgui.h")
add_definitions(-DGLAD)
add_definitions(-DGLM)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ctools EXCLUDE_FROM_ALL)

set_target_properties(ctools PROPERTIES FOLDER 3rdparty/aiekick)
set_target_properties(ctools PROPERTIES LINK_FLAGS "/ignore:4244")