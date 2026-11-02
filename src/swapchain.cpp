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

void VulkanApp::recreateSwapchain() {
  vk::Device vk_device = static_cast<vk::Device>(vkb_device.device);

  vk_device.waitIdle();
  create_swapchain();
  createImageViews();

}


void VulkanApp::cleanupSwapchain() {
  swapChainImageViews.clear();
  //vk::SwapchainKHR swapChain = static_cast<vk::SwapchainKHR>(vkb_swapchain.swapchain);
}
void VulkanApp::create_swapchain() {
  vkb::SwapchainBuilder swapchain_builder{ vkb_device };
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window, &width, &height);

  swapchain_builder.set_desired_extent(
                                       static_cast<uint32_t>(width),
                                       static_cast<uint32_t>(height)
    );
  auto swap_ret = swapchain_builder.build();
  if(!swap_ret){
    std::cerr << "i dont want to write all this";
  }
  vkb_swapchain = swap_ret.value();

  swapChainSurfaceFormat = static_cast<vk::Format>(vkb_swapchain.image_format);
  auto images_ret = vkb_swapchain.get_images();


  vk::SurfaceCapabilitiesKHR capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

  std::cout << "Surface currentExtent: "
  << capabilities.currentExtent.width << " x "
  << capabilities.currentExtent.height << '\n';

  std::cout << "Surface minImageExtent: "
  << capabilities.minImageExtent.width << " x "
  << capabilities.minImageExtent.height << '\n';

  std::cout << "Surface maxImageExtent: "
  << capabilities.maxImageExtent.width << " x "
  << capabilities.maxImageExtent.height << '\n';


  std::cout << "Vk-Bootstrap extent: "
  << vkb_swapchain.extent.width << " x "
  << vkb_swapchain.extent.height << '\n';

  swapChainExtent = vk::Extent2D{
    vkb_swapchain.extent.width,
    vkb_swapchain.extent.height
  };
  swapChainImages.clear();
  for (VkImage image : images_ret.value()) {
    swapChainImages.emplace_back(image);
  }
}

void VulkanApp::createImageViews(){
  assert(swapChainImageViews.empty());


  vk::ImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
  imageViewCreateInfo.format = swapChainSurfaceFormat;
  imageViewCreateInfo.subresourceRange = {
    vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1
  };
  vk::Device vk_device =  static_cast<vk::Device>(vkb_device.device);
  for (auto &image : swapChainImages) {
    imageViewCreateInfo.image =image;
    swapChainImageViews.emplace_back(vk_device.createImageView(imageViewCreateInfo));
  }
}
