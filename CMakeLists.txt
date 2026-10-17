
cmake_minimum_required(VERSION 3.25)

project(VulkanApp LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_executable(VulkanApp
    src/main.cpp
  )


include(FetchContent)
add_subdirectory(vk-bootstrap)

target_link_libraries(VulkanApp PRIVATE
    Vulkan::Vulkan
    glfw
    vk-bootstrap::vk-bootstrap
)
