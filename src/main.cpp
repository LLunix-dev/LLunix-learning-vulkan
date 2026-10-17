#include <iostream>
#include <vulkan/vulkan.hpp>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>



bool init_vulkan () {
  vkb::InstanceBuilder builder;
  auto inst_ret = builder.set_app_name("LLunix's Vulkan Journey").request_validation_layers().use_default_debug_messenger().build();
  if(!inst_ret) {
    std::cerr << "Faileed to creata da Vulkan Instanc";
  };
  vkb::Instance vkb_instance = inst_ret.value();
  return false;
}
bool init_window() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(1028, 1028, "LLunixs Vulkan Journey", NULL, NULL);
  return false;
}

int main() {
  init_vulkan();
  init_window();
}
