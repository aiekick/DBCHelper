set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
  find_package(X11 REQUIRED)

  if (NOT X11_Xi_FOUND)
    message(FATAL_ERROR "X11 Xi library is required")
  endif ()
endif ()

include(cmake/dbcppp.cmake)
include(cmake/glad.cmake)
include(cmake/glfw.cmake)
include(cmake/imgui.cmake)
include(cmake/ctools.cmake)
include(cmake/imguifiledialog.cmake)
include(cmake/tinyxml2.cmake)
include(cmake/freetype.cmake)

