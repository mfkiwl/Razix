// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKSwapchain.h"

#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"
#include "Razix/Platform/API/Vulkan/VKCommandPool.h"
#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKFence.h"
#include "Razix/Platform/API/Vulkan/VKTexture.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include <glfw/glfw3.h>

namespace Razix {
    namespace Graphics {

        VKSwapchain::VKSwapchain(uint32_t width, uint32_t height)
        {
            m_Width  = width;
            m_Height = height;

            // Initialize the swapchain
            Init(m_Width, m_Height);
        }

        VKSwapchain::~VKSwapchain() {}

        void VKSwapchain::Init(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Query the swapchain surface properties
            querySwapSurfaceProperties();

            // Get the color space format for the swapchain images
            m_SurfaceFormat = chooseSurfaceFomat();

            // Choose the presentation mode
            m_PresentMode = choosePresentMode();

            // Choose the swapchain extent
            m_SwapchainExtent = chooseSwapExtent();

            // Create the KHR Swapchain
            createSwapchain();

            // Retrieve and create the swapchain images
            std::vector<VkImage> images = retrieveSwapchainImages();

            // Create image view for the retrieved swapchain images
            std::vector<VkImageView> imageView = createSwapImageViews(images);

            // Encapsulate the swapchain images and image views in a RZTexture2D
            m_SwapchainImageTextures.clear();
            for (uint32_t i = 0; i < m_SwapchainImageCount; i++) {
                VKTexture2D* swapImageTexture = new VKTexture2D(images[i], imageView[i]);
                m_SwapchainImageTextures.push_back(swapImageTexture);
            }

            // Create the sync primitives for each frame + Command Buffers
            createFrameData();
        }

        void VKSwapchain::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Delete the frame data
            for (auto& frame: m_Frames) {
                //frame.mainCommandBuffer->Reset();
                vkDestroySemaphore(VKDevice::Get().getDevice(), frame.presentSemaphore, nullptr);
                vkDestroySemaphore(VKDevice::Get().getDevice(), frame.renderSemaphore, nullptr);
                vkDestroyFence(VKDevice::Get().getDevice(), frame.renderFence->getVKFence(), nullptr);
            }

            for (uint32_t i = 0; i < m_SwapchainImageCount; i++) {
                auto tex = static_cast<RZTexture*>(m_SwapchainImageTextures[i]);
                tex->Release(false);
            }
            m_SwapchainImageTextures.clear();
            vkDestroySwapchainKHR(VKDevice::Get().getDevice(), m_Swapchain, nullptr);
        }

        void VKSwapchain::Flip() {}

        void VKSwapchain::OnResize(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_Width == width && m_Height == height)
                return;

            m_IsResized = true;

            //  Wait for the device to be done executing all the commands
            //VKContext::Get()->waitIdle();

            vkDeviceWaitIdle(VKDevice::Get().getDevice());

            m_Width  = width;
            m_Height = height;

            m_OldSwapChain = m_Swapchain;

            for (uint32_t i = 0; i < m_SwapchainImageCount; i++) {
                auto tex = static_cast<RZTexture*>(m_SwapchainImageTextures[i]);
                tex->Release(false);
                delete m_SwapchainImageTextures[i];
            }
            m_SwapchainImageTextures.clear();
            //vkDestroySwapchainKHR(VKDevice::Get().getDevice(), m_Swapchain, nullptr);

            m_Swapchain = VK_NULL_HANDLE;

            Init(width, height);

            //VKContext::Get()->setSwapchain(this);
        }

        void VKSwapchain::querySwapSurfaceProperties()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the surface capabilities
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &m_SwapSurfaceProperties.capabilities);

            // Get the surface formats supported
            uint32_t formatsCount = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &formatsCount, nullptr);
            m_SwapSurfaceProperties.formats.resize(formatsCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &formatsCount, m_SwapSurfaceProperties.formats.data());

            // Get the available present modes
            uint32_t presentModesCount = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &presentModesCount, nullptr);
            m_SwapSurfaceProperties.presentModes.resize(presentModesCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &presentModesCount, m_SwapSurfaceProperties.presentModes.data());
        }

        VkSurfaceFormatKHR VKSwapchain::chooseSurfaceFomat()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the right color space
            // Get the right image format for the swapchain images to present mode
            for (const auto& format: m_SwapSurfaceProperties.formats) {
                if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    m_ColorFormat = format.format;
                    return format;
                }
            }
            return m_SwapSurfaceProperties.formats[0];
        }

        VkPresentModeKHR VKSwapchain::choosePresentMode()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Choose the right kind of image presentation mode for the  swapchain images
            for (const auto& presentMode: m_SwapSurfaceProperties.presentModes) {
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                    return presentMode;
            }
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D VKSwapchain::chooseSwapExtent()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // choose the best Swapchain resolution to present the image onto
            if (m_SwapSurfaceProperties.capabilities.currentExtent.width != UINT32_MAX)
                return m_SwapSurfaceProperties.capabilities.currentExtent;
            else {
                auto& capabilities = m_SwapSurfaceProperties.capabilities;
                int   width, height;
                glfwGetFramebufferSize((GLFWwindow*) VKContext::Get()->getWindow()->GetNativeWindow(), &width, &height);

                VkExtent2D actualExtent =
                    {
                        static_cast<uint32_t>(width),
                        static_cast<uint32_t>(height)};

                actualExtent.width  = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

                return actualExtent;
            }
        }

        void VKSwapchain::createSwapchain()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the swapchain image count
            m_SwapchainImageCount = m_SwapSurfaceProperties.capabilities.minImageCount + 1;    // For triple buffering
            // Bound checking the swapchain image count only for triple buffer aka 2 frames in flight
            if (m_SwapSurfaceProperties.capabilities.maxImageCount > 0 && m_SwapchainImageCount > m_SwapSurfaceProperties.capabilities.maxImageCount)
                m_SwapchainImageCount = m_SwapSurfaceProperties.capabilities.maxImageCount;
            RAZIX_CORE_TRACE("[Vulkan] Swap images count : {0}", m_SwapchainImageCount);

            // Now create the Swapchain
            VkSwapchainCreateInfoKHR swcCI                            = {};
            swcCI.sType                                               = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swcCI.surface                                             = VKContext::Get()->getSurface();
            swcCI.minImageCount                                       = m_SwapchainImageCount;
            swcCI.imageFormat                                         = m_SurfaceFormat.format;
            swcCI.imageColorSpace                                     = m_SurfaceFormat.colorSpace;
            swcCI.imageExtent                                         = m_SwapchainExtent;
            swcCI.imageArrayLayers                                    = 1;
            swcCI.imageUsage                                          = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
            VKPhysicalDevice::QueueFamilyIndices indices              = VKDevice::Get().getPhysicalDevice().get()->getQueueFamilyIndices();
            uint32_t                             queueFamilyIndices[] = {static_cast<uint32_t>(indices.Graphics), static_cast<uint32_t>(indices.Present)};

            if (indices.Graphics != indices.Present) {
                swcCI.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
                swcCI.queueFamilyIndexCount = 2;
                swcCI.pQueueFamilyIndices   = queueFamilyIndices;
            } else {
                swcCI.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
                swcCI.queueFamilyIndexCount = 0;          // Optional
                swcCI.pQueueFamilyIndices   = nullptr;    // Optional
            }

            swcCI.preTransform   = m_SwapSurfaceProperties.capabilities.currentTransform;
            swcCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swcCI.clipped        = VK_TRUE;
            swcCI.oldSwapchain   = m_OldSwapChain;

            // Now actually create the swapchainManager
            if (VK_CHECK_RESULT(vkCreateSwapchainKHR(VKDevice::Get().getDevice(), &swcCI, nullptr, &m_Swapchain)))
                RAZIX_CORE_ERROR("[Vulkan] Cannot create swapchain!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Swapchain created successfully!");

            if (m_OldSwapChain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(VKDevice::Get().getDevice(), m_OldSwapChain, VK_NULL_HANDLE);
                m_OldSwapChain = VK_NULL_HANDLE;
            }
        }

        std::vector<VkImage> VKSwapchain::retrieveSwapchainImages()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            std::vector<VkImage> swapImages;

            uint32_t swapImageCount = 0;
            vkGetSwapchainImagesKHR(VKDevice::Get().getDevice(), m_Swapchain, &swapImageCount, nullptr);
            RAZIX_CORE_ASSERT((swapImageCount == m_SwapchainImageCount), "[Vulkan] Swapimage count doesn't match!");

            swapImages.resize(m_SwapchainImageCount);
            if (VK_CHECK_RESULT(vkGetSwapchainImagesKHR(VKDevice::Get().getDevice(), m_Swapchain, &m_SwapchainImageCount, swapImages.data())))
                RAZIX_CORE_ERROR("[Vulkan] Cannot retrieve swapchain images!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Swapchain images {0} have been retrieved successfully!", swapImageCount);

            return swapImages;
        }

        std::vector<VkImageView> VKSwapchain::createSwapImageViews(std::vector<VkImage> swapImages)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            std::vector<VkImageView> swapchainImageViews;

            swapchainImageViews.resize(m_SwapchainImageCount);
            for (size_t i = 0; i < m_SwapchainImageCount; i++) {
                // Create the image view with the required properties
                VkImageViewCreateInfo imvCI{};
                imvCI.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                imvCI.image                           = swapImages[i];
                imvCI.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
                imvCI.format                          = m_SurfaceFormat.format;
                imvCI.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                imvCI.subresourceRange.baseMipLevel   = 0;
                imvCI.subresourceRange.levelCount     = 1;
                imvCI.subresourceRange.baseArrayLayer = 0;
                imvCI.subresourceRange.layerCount     = 1;
                if (VK_CHECK_RESULT(vkCreateImageView(VKDevice::Get().getDevice(), &imvCI, nullptr, &swapchainImageViews[i])))
                    RAZIX_CORE_ERROR("[Vulkan] Cannot create swap image view!");
                else
                    RAZIX_CORE_TRACE("[Vulkan] Swap Image view (id= {0} ) succesfully created!", i);
            }
            return swapchainImageViews;
        }

        void VKSwapchain::createFrameData()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            for (uint32_t i = 0; i < m_SwapchainImageCount; i++) {
                if (!m_Frames[i].renderFence) {
                    VkSemaphoreCreateInfo semaphoreInfo = {};
                    semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                    semaphoreInfo.pNext                 = nullptr;

                    if (m_Frames[i].presentSemaphore == VK_NULL_HANDLE)
                        VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().getDevice(), &semaphoreInfo, nullptr, &m_Frames[i].presentSemaphore));
                    if (m_Frames[i].renderSemaphore == VK_NULL_HANDLE)
                        VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().getDevice(), &semaphoreInfo, nullptr, &m_Frames[i].renderSemaphore));

                    m_Frames[i].renderFence = CreateRef<VKFence>(true);
                }
            }
        }

        void VKSwapchain::acquireNextImage()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            uint32_t nextCmdBufferIndex = (m_CurrentBuffer + 1) % m_SwapchainImageCount;
            {
                auto result = vkAcquireNextImageKHR(VKDevice::Get().getDevice(), m_Swapchain, UINT64_MAX, m_Frames[nextCmdBufferIndex].presentSemaphore, VK_NULL_HANDLE, &m_AcquireImageIndex);
                if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
                    RAZIX_CORE_TRACE("[Vulkan] Acquire Image result : {0}", result == VK_ERROR_OUT_OF_DATE_KHR ? "Out of Date" : "SubOptimal");

                    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                        /*             std::unique_lock<std::mutex> lk(RZApplication::m);
                        RZApplication::halt_execution.wait(lk, [] {
                            return RZApplication::ready_for_execution;
                        });*/
                        //OnResize(m_Width, m_Height);
                        //acquireNextImage();
                        vkDeviceWaitIdle(VKDevice::Get().getDevice());
                    }
                    return;
                } else if (result != VK_SUCCESS)
                    RAZIX_CORE_ERROR("[Vulkan] Failed to acquire swap chain image!");

                m_CurrentBuffer = nextCmdBufferIndex;
                return;
            }
        }

        void VKSwapchain::queueSubmit(CommandQueue& commandQueue)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            //if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
            //    vkWaitForFences(VKDevice::Get().getDevice(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
            //imagesInFlight[imageIndex] = inFlightFences[currentFrame];

            auto&        frameData        = getCurrentFrameSyncData();
            VkSubmitInfo submitInfo       = {};
            submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext              = VK_NULL_HANDLE;
            submitInfo.commandBufferCount = static_cast<uint32_t>(commandQueue.size());

            std::vector<VkCommandBuffer> cmdBuffs;
            for (size_t i = 0; i < submitInfo.commandBufferCount; i++)
                cmdBuffs.push_back(*((VkCommandBuffer*) commandQueue[i]->getAPIBuffer()));

            submitInfo.pCommandBuffers     = cmdBuffs.data();
            VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

            submitInfo.pWaitDstStageMask = &waitStage;

            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores    = &frameData.presentSemaphore;

            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = &frameData.renderSemaphore;

            frameData.renderFence->reset();

            {
                VK_CHECK_RESULT(vkQueueSubmit(VKDevice::Get().getGraphicsQueue(), 1, &submitInfo, frameData.renderFence->getVKFence()));
            }

            frameData.renderFence->wait();
        }

        void VKSwapchain::present()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto& frameData = getCurrentFrameSyncData();

            VkPresentInfoKHR present{};
            present.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present.pNext              = VK_NULL_HANDLE;
            present.swapchainCount     = 1;
            present.pSwapchains        = &m_Swapchain;
            present.pImageIndices      = &m_AcquireImageIndex;
            present.waitSemaphoreCount = 1;
            present.pWaitSemaphores    = &frameData.renderSemaphore;
            present.pResults           = VK_NULL_HANDLE;
            auto error                 = vkQueuePresentKHR(VKDevice::Get().getPresentQueue(), &present);

            //if (error == VK_ERROR_OUT_OF_DATE_KHR || error == VK_SUBOPTIMAL_KHR || m_IsResized) {
            //    m_IsResized = !m_IsResized;
            //    //VKContext::Get()->waitIdle();
            //    vkDeviceWaitIdle(VKDevice::Get().getDevice());
            //    for (uint32_t i = 0; i < m_SwapchainImageCount; i++) {
            //        auto tex = static_cast<RZTexture*>(m_SwapchainImageTextures[i]);
            //        tex->Release(false);
            //        delete m_SwapchainImageTextures[i];
            //    }
            //    m_SwapchainImageTextures.clear();
            //    vkDestroySwapchainKHR(VKDevice::Get().getDevice(), m_Swapchain, nullptr);
            //    Init(m_Width, m_Height);
            //    return;
            //}

            if (error == VK_ERROR_OUT_OF_DATE_KHR) {
                vkDeviceWaitIdle(VKDevice::Get().getDevice());
                RAZIX_CORE_ERROR("[Vulkan] Swapchain out of date");
            } else if (error == VK_SUBOPTIMAL_KHR)
                RAZIX_CORE_ERROR("[Vulkan] Swapchain suboptimal");
            else
                VK_CHECK_RESULT(error);
        }
    }    // namespace Graphics
}    // namespace Razix
