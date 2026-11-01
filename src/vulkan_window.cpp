#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include "renderer.hpp"


void VulkanApp::create_instance () {
    vkb::InstanceBuilder builder;

    auto inst_ret = builder.set_app_name("LLunix's Vulkan Journey").require_api_version(1, 4, 0).request_validation_layers().use_default_debug_messenger().build();
    if(!inst_ret) {
      std::cerr << "Faileed to creata da Vulkan Instanc";
    };
    vkb_instance = inst_ret.value();
  }

void VulkanApp::init_window() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window = glfwCreateWindow(600, 600, "LLunix's Vulkan Engine", NULL, NULL);
}
void VulkanApp::create_surface(){
  VkResult glfw_result = glfwCreateWindowSurface(vkb_instance, window, nullptr, &surface);
  if (glfw_result != VK_SUCCESS) {
    std::cerr << "Faiilead to createa da window surfase";
  }
}
