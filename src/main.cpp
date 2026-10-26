
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <iostream>
#include <vulkan/vulkan.hpp>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

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
    mainLoop();
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
  vk::Extent2D swapChainExtent;
  vk::PhysicalDevice physicalDevice;
  vk::PipelineLayout pipelineLayout = nullptr;
  vk::Pipeline graphicsPipeline = nullptr;
  vk::CommandPool commandPool = nullptr;
  vk::CommandBuffer commandBuffer = nullptr;

  void init_vulkan() {
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

  }
  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      drawFrame();
    }
  }

  void drawFrame() {
    //todo
  }

  void create_instance () {
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
    vk::PhysicalDeviceVulkan13Features features13{};
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

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

    swapChainExtent = chooseSwapExtent(surfaceCapabilities);

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
    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    vk::Viewport viewport{0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0f, 1.0f};
    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;


    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.depthClampEnable = vk::False;
    rasterizer.rasterizerDiscardEnable = vk::False;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = vk::False;
    rasterizer.lineWidth = 1.0f;
    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.sampleShadingEnable = vk::False;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.blendEnable = vk::False;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.colorBlendOp        = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp        = vk::BlendOp::eAdd;
    colorBlendAttachment.colorWriteMask      = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = vk::False;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    std::vector<vk::DynamicState>      dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();


    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    vk::Device vk_device = static_cast<vk::Device>(vkb_device.device);
    pipelineLayout = vk_device.createPipelineLayout(pipelineLayoutInfo);


    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
    pipelineRenderingCreateInfo.colorAttachmentCount =1 ;
    pipelineRenderingCreateInfo.pColorAttachmentFormats = &swapChainSurfaceFormat;

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {{},{}};
    auto& pipelineInfo = pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>();
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = shaderStages;
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.layout              = pipelineLayout;
    pipelineInfo.renderPass          = nullptr;

    auto& renderingInfo = pipelineCreateInfoChain.get<vk::PipelineRenderingCreateInfo>();
    renderingInfo.colorAttachmentCount    = 1;
    renderingInfo.pColorAttachmentFormats = &swapChainSurfaceFormat;

    graphicsPipeline = vk_device.createGraphicsPipeline(nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()).value;
  }

  void createCommandPool() {
    vk::Device vk_device = static_cast<vk::Device>(vkb_device.device);
    uint32_t queueIndex = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueIndex;

    commandPool = vk_device.createCommandPool(poolInfo);


  }
  void createCommandBuffer() {
    vk::Device vk_device = static_cast<vk::Device>(vkb_device.device);
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount =1;
    commandBuffer = vk_device.allocateCommandBuffers(allocInfo).front();

  }

  void recordCommandBuffer(uint32_t imageIndex){
    vk::Result result = commandBuffer.begin({});
    transition_image_layout(imageIndex,
                            vk::ImageLayout::eUndefined,
                            vk::ImageLayout::eColorAttachmentOptimal,
                            {},
                            vk::AccessFlagBits2::eColorAttachmentWrite,
                            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                            vk::PipelineStageFlagBits2::eColorAttachmentOutput);

    vk::ClearValue clearColor = vk::ClearColorValue(0.0f,0.0f,0.0f,1.0f);
    vk::RenderingAttachmentInfo attachmentInfo = {};
    attachmentInfo.imageView = swapChainImageViews[imageIndex];
    attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
    attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
    attachmentInfo.clearValue = clearColor;
    vk::RenderingInfo renderingInfo = {};
    renderingInfo.renderArea = {.offset = {0,0}, .extent = swapChainExtent};
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &attachmentInfo;
    commandBuffer.beginRendering(renderingInfo);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
    commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height)));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0,0), swapChainExtent));
    commandBuffer.draw(3, 1, 0, 0);
    commandBuffer.endRendering();

    commandBuffer.endRendering();
    transition_image_layout(imageIndex,
                            vk::ImageLayout::eColorAttachmentOptimal,
                            vk::ImageLayout::ePresentSrcKHR,
                            vk::AccessFlagBits2::eColorAttachmentWrite,
                            {},
                            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                            vk::PipelineStageFlagBits2::eBottomOfPipe);
    commandBuffer.end();
  }

  void transition_image_layout(uint32_t imageIndex,
                               vk::ImageLayout old_layout,
                               vk::ImageLayout new_layout,
                               vk::AccessFlags2 src_access_mask,
                               vk::AccessFlags2 dst_access_mask,
                               vk::PipelineStageFlags2 src_stage_mask,
                               vk::PipelineStageFlags2 dst_stage_mask) {

    vk::ImageMemoryBarrier2 barrier = {};
    barrier.srcStageMask = src_stage_mask;
    barrier.srcAccessMask = src_access_mask;
    barrier.dstStageMask = dst_stage_mask;
    barrier.dstAccessMask = dst_access_mask;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = swapChainImages[imageIndex],
    barrier.subresourceRange = vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
    vk::DependencyInfo dependency_info = {};
    dependency_info.dependencyFlags = {};
    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers = &barrier;

    commandBuffer.pipelineBarrier2(dependency_info);



  }
  void cleanup(){


    vk::Device vk_device = static_cast<vk::Device>(vkb_device.device);
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
  vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities)
  {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
      return capabilities.currentExtent;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    return {
      std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
      std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)};
  }

};


int main() {
  std::cout << "C++ version: " << __cplusplus << '\n';
  VulkanApp app;
  app.run();
  return 0;
}
