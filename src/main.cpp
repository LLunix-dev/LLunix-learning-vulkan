#include <iostream>
#include <vulkan/vulkan.hpp>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>



class VulkanApp{
  public:
  void run(){
    init_vulkan();
    init_window();
    create_surface();
    select_phys_device();
    create_graphics_queue();
    create_swapchain();

    cleanup();
  }
  private:
  GLFWwindow *window = nullptr;
  vkb::Instance vkb_instance;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  vkb::Device vkb_device;
  vkb::Swapchain vkb_swapchain;

  void init_vulkan () {
    vkb::InstanceBuilder builder;

    auto inst_ret = builder.set_app_name("LLunix's Vulkan Journey").request_validation_layers().use_default_debug_messenger().build();
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
    vkb::PhysicalDeviceSelector selector{ vkb_instance };
    auto phys_ret = selector.set_surface(surface).select();
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

  }

  void cleanup(){
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
