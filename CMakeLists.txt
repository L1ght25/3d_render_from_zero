cmake_minimum_required(VERSION 3.23)
project(3d_render)
find_package(SFML 2 COMPONENTS graphics REQUIRED)

set(CMAKE_CXX_STANDARD 20)

#include_directories(/opt/homebrew/Cellar/sfml/2.5.1_2/include)
#link_libraries(/opt/homebrew/Cellar/sfml/2.5.1_2/lib)

add_executable(3d_render main.cpp bitmap.h starfield.h RenderContext.h Vector4d.h Matrix4d.h Edge.h mesh.h)
target_link_libraries(3d_render PRIVATE
        sfml-graphics
        sfml-window
        sfml-system)