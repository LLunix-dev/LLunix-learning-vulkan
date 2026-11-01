#include <vulkan/vulkan.hpp>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include "renderer.hpp"

void VulkanApp::cleanup() {
  vk::Device vk_device = static_cast<vk::Device>(vkb_device.device);

  vk_device.destroyCommandPool(commandPool);
  vk_device.destroySemaphore(presentCompleteSemaphore);
  for(vk::Semaphore &semaphore : renderFinishedSemaphores) {
    vk_device.destroySemaphore(semaphore);
  }
  vk_device.destroyFence(drawFence);

  vk_device.destroyPipeline(graphicsPipeline);
  vk_device.destroyPipelineLayout(pipelineLayout);
  vk_device.destroyShaderModule(shaderModule);

  for (auto imageView : swapChainImageViews) {
    vk_device.destroyImageView(imageView);
  }

  swapChainImageViews.clear();

  vkb::destroy_swapchain(vkb_swapchain);

  vkb::destroy_device(vkb_device);
  vkb::destroy_surface(vkb_instance, surface);
  vkb::destroy_instance(vkb_instance);

  glfwDestroyWindow(window);
  glfwTerminate();
}
