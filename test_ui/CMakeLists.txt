cmake_minimum_required(VERSION 3.16)
project(InfernoUITest)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
  GIT_SHALLOW    TRUE
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND  OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ImGui (project copy only has Android backend; need desktop backends)
FetchContent_Declare(
  imgui_src
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.91.8
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui_src)

add_executable(inferno_ui_test
  main.cpp
  ${imgui_src_SOURCE_DIR}/imgui.cpp
  ${imgui_src_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_src_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_src_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_src_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_src_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(inferno_ui_test PRIVATE
  ${imgui_src_SOURCE_DIR}
  ${imgui_src_SOURCE_DIR}/backends
  ${glfw_SOURCE_DIR}/include
)

target_link_libraries(inferno_ui_test PRIVATE glfw)
