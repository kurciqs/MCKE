cmake_minimum_required(VERSION 3.18)
project(Minecraft)
set(CMAKE_CXX_STANDARD 17)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/yaml-cpp/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

set(MAIN ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)


set(
        SOURCE_FILES
        ${MAIN}
        src/Graphics/Shader.cpp
        src/Window/Window.cpp
        src/Window/Input.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/noise/SimplexNoise.cpp
        src/Graphics/Camera.cpp
        src/World/Chunk.cpp
        src/World/World.cpp
        src/App/App.cpp
        src/Graphics/TextureParser.cpp
        src/Gameplay/Player.cpp
        src/Gameplay/Physics.cpp
)


set(
        HEADER_FILES
        src/Gameplay/Components.h
        src/Graphics/Vertex.h
        src/Graphics/Shader.h
        src/Window/Window.h
        src/Window/Input.h
        src/Graphics/Camera.h
        src/World/Chunk.h
        src/World/World.h
        src/App/App.h
        src/Graphics/TextureParser.h
        src/Gameplay/ECS.h
        src/Gameplay/Player.h
        src/Gameplay/Physics.h
)

set(GLFW_BUILD_EXAMPLES false)
set(GLFW_BUILD_TESTS false)

add_executable(Minecraft ${SOURCE_FILES} ${HEADER_FILES})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs)

set(YAML_CPP_BUILD_TESTS false)
set(YAML_CPP_BUILD_TOOLS false)
set(YAML_CPP_BUILD_CONTRIB false)
add_subdirectory(include/yaml-cpp/)

target_include_directories(Minecraft
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw-3.3.4/include")

find_package(OpenGL REQUIRED)
target_link_libraries(Minecraft PRIVATE ${OPENGL_gl_LIBRARY})
target_link_libraries(Minecraft PRIVATE glad)
target_link_libraries(Minecraft PRIVATE glfw)
target_link_libraries(Minecraft PRIVATE yaml-cpp)
target_link_libraries(Minecraft PRIVATE -static gcc stdc++)
IF (WIN32)
        target_link_libraries(Minecraft PRIVATE -static winpthread)
ENDIF()
