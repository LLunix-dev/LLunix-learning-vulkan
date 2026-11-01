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

void VulkanApp::run() {
  init_vulkan();
  mainLoop();
  cleanup();
}

void VulkanApp::init_vulkan() {
    create_instance();
    init_window();
    create_surface();
    select_phys_device();
    create_graphics_queue();
    create_swapchain();
    createImageViews();
    createGraphicsPipeline();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
  }


void VulkanApp::mainLoop() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    drawFrame();
  }
  vk::Device vkDevice = static_cast<vk::Device>(vkb_device.device);
  vkDevice.waitIdle();
}







void VulkanApp::drawFrame() {
  vk::Device vk_device = static_cast<vk::Device>(vkb_device.device);

  auto fenceResult = vk_device.waitForFences(1, &drawFence, vk::True, UINT64_MAX);
  if (fenceResult != vk::Result::eSuccess) {
    throw std::runtime_error("Failed to wait for Fence idk");
  }
  vk_device.resetFences(drawFence);
  vk::SwapchainKHR swapChain = vkb_swapchain.swapchain;
  auto [result, imageIndex] = vk_device.acquireNextImageKHR(swapChain, UINT64_MAX, presentCompleteSemaphore, nullptr);
  recordCommandBuffer(imageIndex);

  vk::Semaphore renderFinished =
    renderFinishedSemaphores.at(imageIndex);

  vk::PipelineStageFlags waitDestinationStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput );
  vk::SubmitInfo submitInfo = {};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
  submitInfo.pWaitDstStageMask = &waitDestinationStageMask;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &renderFinished;

  auto resetFenceResult = vk_device.resetFences(1, &drawFence);
  graphics_queue.submit(submitInfo, drawFence);

  vk::PresentInfoKHR presentInfoKHR = {};
  presentInfoKHR.waitSemaphoreCount = 1;
  presentInfoKHR.pWaitSemaphores = &renderFinished;
  presentInfoKHR.swapchainCount = 1;
  presentInfoKHR.pSwapchains = &swapChain;
  presentInfoKHR.pImageIndices = &imageIndex;
  result = graphics_queue.presentKHR(presentInfoKHR);
}
