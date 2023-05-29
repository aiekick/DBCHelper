message("USE OF GLFW3 as GUI")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glfw EXCLUDE_FROM_ALL)

set_target_properties(glfw PROPERTIES FOLDER 3rdparty/glfw3) # Override standard 'GLFW3' subfolder
set_target_properties(update_mappings PROPERTIES FOLDER 3rdparty/glfw3) # Override standard 'GLFW3' subfolder

set(GLFW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glfw/include)
set(GLFW_DEFINITIONS -DGLFW_INCLUDE_NONE)
set(GLFW_LIBRARIES ${GLFW_LIBRARIES} glfw)

add_definitions(-DUSE_GLFW3)
add_definitions(-D_GLFW_USE_CONFIG_H)
add_definitions(${GLFW_DEFINITIONS})