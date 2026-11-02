#pragma once

#include "vulkan/vulkan.hpp"
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

class VulkanApp{
public:
  void run();

private:
  GLFWwindow *window = nullptr;
  vkb::Instance vkb_instance;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  vkb::Device vkb_device;
  vk::Queue graphics_queue;
  vkb::Swapchain vkb_swapchain;
  std::vector<vk::ImageView> swapChainImageViews;
  vk::Format swapChainSurfaceFormat;
  std::vector<vk::Image> swapChainImages;
  vk::ShaderModule shaderModule;
  vk::Extent2D swapChainExtent;
  vk::PhysicalDevice physicalDevice;
  vk::PipelineLayout pipelineLayout = nullptr;
  vk::Pipeline graphicsPipeline = nullptr;
  vk::CommandPool commandPool = nullptr;
  vk::CommandBuffer commandBuffer = nullptr;
  vk::Semaphore presentCompleteSemaphore = nullptr;
  std::vector<vk::Semaphore> renderFinishedSemaphores;
  vk::Fence drawFence = nullptr;

  void init_vulkan();
  void mainLoop();
  void createSyncObjects();
  void drawFrame();
  void create_instance();
  void init_window();
  void create_surface();
  void select_phys_device();
  void create_graphics_queue();
  void create_swapchain();
  void createImageViews();

  [[nodiscard]] vk::ShaderModule createShaderModule(const std::vector<char>& code);

  void createGraphicsPipeline();
  void createCommandPool();
  void createCommandBuffer();
  void recordCommandBuffer(uint32_t imageIndex);
  void transition_image_layout(uint32_t imageIndex,
                               vk::ImageLayout old_layout,
                               vk::ImageLayout new_layout,
                               vk::AccessFlags2 src_access_mask,
                               vk::AccessFlags2 dst_access_mask,
                               vk::PipelineStageFlags2 src_stage_mask,
                               vk::PipelineStageFlags2 dst_stage_mask);
  void recreateSwapchain();
  void cleanupSwapchain();

  void cleanup();
  vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities);
};
