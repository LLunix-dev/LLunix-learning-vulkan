#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include "renderer.hpp"

void VulkanApp::createSyncObjects() {
  vk::Device vk_device = static_cast<vk::Device>(vkb_device.device);
  presentCompleteSemaphore = vk_device.createSemaphore(vk::SemaphoreCreateInfo());
  renderFinishedSemaphores.resize(swapChainImages.size());

  for (auto& semaphore : renderFinishedSemaphores) {
    semaphore = vk_device.createSemaphore(vk::SemaphoreCreateInfo{});
  }
  vk::FenceCreateInfo fenceCreateInfo = {};
  fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;
  drawFence = vk_device.createFence(fenceCreateInfo);

}
