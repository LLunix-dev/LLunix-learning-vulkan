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


void VulkanApp::select_phys_device(){
  vk::PhysicalDeviceVulkan11Features features11{};
  features11.shaderDrawParameters = VK_TRUE;
  vk::PhysicalDeviceVulkan13Features features13{};
  features13.synchronization2 = VK_TRUE;
  features13.dynamicRendering = VK_TRUE;
  vkb::PhysicalDeviceSelector selector{ vkb_instance };
  auto phys_ret = selector.set_surface(surface).set_required_features_11(features11).set_required_features_13(features13).select();
  physicalDevice = static_cast<vk::PhysicalDevice>(phys_ret.value());
  std::cout << phys_ret.value().name;
  if (!phys_ret) {
    std::cerr << "Failed to select Vulkan Physical Device. Error: " << phys_ret.error().message() << "\n";
    if (phys_ret.error() == vkb::PhysicalDeviceError::no_suitable_device) {
      const auto& detailed_reasons = phys_ret.detailed_failure_reasons();
      if (!detailed_reasons.empty()) {
        std::cerr << "GPU Selection failure reasons:\n";
        for (const std::string& reason : detailed_reasons) {
          std::cerr << reason << "\n";
        }
      }
    }
  }


  vkb::DeviceBuilder device_builder{ phys_ret.value()};

  auto dev_ret = device_builder.build();
  if(!dev_ret) {
    std::cerr << "Faileet do crate da Vulkan devisa";
  }
  vkb_device = dev_ret.value();


  VkDevice device = vkb_device.device;
}
void VulkanApp::create_graphics_queue() {
  auto ghraphics_queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);
  if (!ghraphics_queue_ret) {
    std::cerr << "Failet do ged da ghraphics que";
  }
  graphics_queue = ghraphics_queue_ret.value();
}
