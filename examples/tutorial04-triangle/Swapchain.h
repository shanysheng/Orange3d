#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

extern VkSurfaceKHR surface;
extern VkQueue presentQueue;

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

extern VkSwapchainKHR swapChain;
extern std::vector<VkImage> swapChainImages;
extern VkFormat swapChainImageFormat;
extern VkExtent2D swapChainExtent;


void createSwapChain(GLFWwindow* pwindow, VkPhysicalDevice pphysicalDev, VkDevice logicaldevice);

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

VkExtent2D chooseSwapExtent(GLFWwindow* pwindow, const VkSurfaceCapabilitiesKHR& capabilities);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice pphysicalDev, VkSurfaceKHR psurface);

void createSurface(VkInstance pinst, GLFWwindow* pwindow);
void cleanupSurface(VkInstance pinst, VkDevice logicaldevice);