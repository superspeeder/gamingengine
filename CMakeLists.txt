cmake_minimum_required(VERSION 3.30)
project(gaming_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(FetchContent)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.15.3)
set(SPDLOG_USE_STD_FORMAT ON)
FetchContent_MakeAvailable(glm spdlog)

file(GLOB_RECURSE GAMING_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.c)
add_executable(gaming ${GAMING_SOURCES}
        src/engine/os.cpp
        src/engine/os.hpp
        src/engine/logging.hpp
        src/engine/logging.cpp
        src/engine/render/render_device.cpp
        src/engine/render/render_device.hpp
        src/engine/scene/scene.cpp
        src/engine/scene/scene.hpp)
target_include_directories(gaming PRIVATE src/)
target_link_libraries(gaming PRIVATE glfw vulkan glm::glm spdlog::spdlog)
target_compile_definitions(gaming PRIVATE GLFW_INCLUDE_NONE GLFW_INCLUDE_VULKAN GLM_ENABLE_EXPERIMENTAL VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)


