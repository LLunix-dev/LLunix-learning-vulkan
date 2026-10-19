
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

  find_program(SLANGC_EXECUTABLE NAMES slangc REQUIRED)

set(SHADER_SLANG_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.slang
)

function(add_slang_shader_target TARGET)
  cmake_parse_arguments(SHADER "" "" "SOURCES" ${ARGN})

  set(SHADERS_DIR ${CMAKE_BINARY_DIR}/shaders)
  set(ENTRY_POINTS -entry vertMain -entry fragMain)

  file(MAKE_DIRECTORY ${SHADERS_DIR})

  add_custom_command(
      OUTPUT ${SHADERS_DIR}/slang.spv
      COMMAND ${SLANGC_EXECUTABLE}
          ${SHADER_SOURCES}
          -target spirv
          -profile spirv_1_4
          -emit-spirv-directly
          -fvk-use-entrypoint-name
          ${ENTRY_POINTS}
          -o ${SHADERS_DIR}/slang.spv
      DEPENDS ${SHADER_SOURCES}
      COMMENT "Compiling Slang Shaders"
      VERBATIM
  )

  add_custom_target(${TARGET}
      DEPENDS ${SHADERS_DIR}/slang.spv
  )
endfunction()

add_slang_shader_target( foo SOURCES ${SHADER_SLANG_SOURCES})
add_dependencies(VulkanApp foo)
