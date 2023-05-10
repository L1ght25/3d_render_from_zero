cmake_minimum_required(VERSION 3.23)
project(3d_render)
find_package(SFML 2 COMPONENTS graphics REQUIRED)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(3d_render
                main.cpp
                src/geometric_primitives/4d_primitives.cpp
                src/geometric_primitives/Vertex.cpp
                src/load_models/Object3d.cpp
                src/rendering/rendering_helpers/Gradients.cpp
                src/rendering/rendering_helpers/Edge.cpp
                src/rendering/RenderContext.cpp
                src/rendering/bitmap/bitmap.cpp
                src/user_api/api.cpp
                src/camera/Camera.cpp)
target_link_libraries(3d_render PRIVATE
        sfml-graphics
        sfml-window
        sfml-system)