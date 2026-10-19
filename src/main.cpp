
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <iostream>
#include <vulkan/vulkan.hpp>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <fstream>

static std::vector<char> readFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if(!file.is_open()) {
    throw std::runtime_error("Faileeed to opan da fielee");
  }
  std::vector<char> buffer(file.tellg());
  file.seekg(0, std::ios::beg);
  file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
  file.close();
  return buffer;
}


class VulkanApp{
  public:
  void run(){
    init_vulkan();
    init_window();
    create_surface();
    select_phys_device();
    create_graphics_queue();
    create_swapchain();
    createImageViews();
    createGraphicsPipeline();

    cleanup();
  }
  private:
  GLFWwindow *window = nullptr;
  vkb::Instance vkb_instance;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  vkb::Device vkb_device;
  vkb::Swapchain vkb_swapchain;
  std::vector<vk::ImageView> swapChainImageViews;
  vk::Format swapChainSurfaceFormat;
  std::vector<vk::Image> swapChainImages;
  vk::ShaderModule shaderModule;

  void init_vulkan () {
    vkb::InstanceBuilder builder;

    auto inst_ret = builder.set_app_name("LLunix's Vulkan Journey").require_api_version(1, 4, 0).request_validation_layers().use_default_debug_messenger().build();
    if(!inst_ret) {
      std::cerr << "Faileed to creata da Vulkan Instanc";
    };
    vkb_instance = inst_ret.value();

  }

  void init_window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(1028, 1028, "LLunixs Vulkan Journey", NULL, NULL);
  }
  void create_surface(){
    VkResult glfw_result = glfwCreateWindowSurface(vkb_instance, window, nullptr, &surface);
    if (glfw_result != VK_SUCCESS) {
      std::cerr << "Faiilead to createa da window surfase";
    }
  }
  void select_phys_device(){
    vk::PhysicalDeviceVulkan11Features features11{};
    features11.shaderDrawParameters = VK_TRUE;
    vkb::PhysicalDeviceSelector selector{ vkb_instance };
    auto phys_ret = selector.set_surface(surface).set_required_features_11(features11).select();
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
  void create_graphics_queue() {
    auto ghraphics_queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);
    if (!ghraphics_queue_ret) {
      std::cerr << "Failet do ged da ghraphics que";
    }
    VkQueue graphics_queue = ghraphics_queue_ret.value();
  }

  void create_swapchain() {
    vkb::SwapchainBuilder swapchain_builder{ vkb_device };

    auto swap_ret = swapchain_builder.build();
    if(!swap_ret){
      std::cerr << "i dont want to write all this";
    }
    vkb_swapchain = swap_ret.value();

    swapChainSurfaceFormat = static_cast<vk::Format>(vkb_swapchain.image_format);
    auto images_ret = vkb_swapchain.get_images();

    for (VkImage image : images_ret.value()) {
      swapChainImages.emplace_back(image);
    }
  }

  void createImageViews(){
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

  [[nodiscard]] vk::ShaderModule createShaderModule(const std::vector<char>& code) const {
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    vk::Device vk_device =  static_cast<vk::Device>(vkb_device.device);
    vk::ShaderModule shaderModule = vk_device.createShaderModule(createInfo);
    return shaderModule;
  }

  void createGraphicsPipeline() {
    auto shaderCode = readFile("shaders/slang.spv");
    shaderModule = createShaderModule(shaderCode);
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = shaderModule;
    vertShaderStageInfo.pName = "vertMain";


    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = shaderModule;
    fragShaderStageInfo.pName = "fragMain";
    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo};

  }




  void cleanup(){


    vk::Device vk_device = static_cast<vk::Device>(vkb_device.device);

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

};


int main() {
  VulkanApp app;
  app.run();
  return 0;
}
