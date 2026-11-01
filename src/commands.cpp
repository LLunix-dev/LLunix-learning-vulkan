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

void VulkanApp::transition_image_layout(uint32_t imageIndex,
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
void VulkanApp::createCommandPool() {
  vk::Device vk_device = static_cast<vk::Device>(vkb_device.device);
  uint32_t queueIndex = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
  vk::CommandPoolCreateInfo poolInfo{};
  poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
  poolInfo.queueFamilyIndex = queueIndex;

  commandPool = vk_device.createCommandPool(poolInfo);


}
void VulkanApp::createCommandBuffer() {
  vk::Device vk_device = static_cast<vk::Device>(vkb_device.device);
  vk::CommandBufferAllocateInfo allocInfo{};
  allocInfo.commandPool = commandPool;
  allocInfo.level = vk::CommandBufferLevel::ePrimary;
  allocInfo.commandBufferCount =1;
  commandBuffer = vk_device.allocateCommandBuffers(allocInfo).front();

}

void VulkanApp::recordCommandBuffer(uint32_t imageIndex){
  vk::CommandBufferBeginInfo beginInfo{};

  commandBuffer.reset();
  commandBuffer.begin(beginInfo);
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
  transition_image_layout(imageIndex,
                          vk::ImageLayout::eColorAttachmentOptimal,
                          vk::ImageLayout::ePresentSrcKHR,
                          vk::AccessFlagBits2::eColorAttachmentWrite,
                          {},
                          vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                          vk::PipelineStageFlagBits2::eBottomOfPipe);
    commandBuffer.end();
  }
