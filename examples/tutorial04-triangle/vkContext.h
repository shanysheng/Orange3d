#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

// ����vulkanӦ������Ҫ��windows
// ����vulkan instanceʵ��������ָ������Ҫ��vulkanʵ����չ
// ����window��instance����ȡ����Ҫ��surface����
// ѡ�������豸����
// �����߼��豸���󣬿���ָ���߼��豸����Ҫ����չ�ӿڣ�����swapchain
// �����߼��豸��Ӧ��swapchain��swapchainimage
// ������Ⱦframebuffer��framebuffer��Ҫͨ��image viewȥ����swap chain image


// command pool ���� command buffer
// command buffer ��render pass�� frame buffer�� graphic pipeline
// render pass ָ�� color attachment�Ĳ�����ʽ
// frame bufferָ��swap chain�е�image view -��swap chain��Ӧ��image
// graphic pipelineָ��fixed pipeline�е�״̬��shader module
//

const int MAX_FRAMES_IN_FLIGHT = 2;


class vkContext
{
public:
	vkContext() {
		window = nullptr;
		instance = nullptr;
		physicalDevice = VK_NULL_HANDLE;
		currentFrame = 0;
	}

	virtual ~vkContext() {

	}

	GLFWwindow*					window;
	VkInstance					instance;

	VkPhysicalDevice			physicalDevice;
	VkDevice					logicaldevice;
	VkQueue						graphicsQueue;

	VkSurfaceKHR				surface;
	VkQueue						presentQueue;

	VkSwapchainKHR				swapChain;
	std::vector<VkImage>		swapChainImages;
	VkFormat					swapChainImageFormat;
	VkExtent2D					swapChainExtent;
	std::vector<VkImageView>	swapChainImageViews;
	std::vector<VkFramebuffer>	swapChainFramebuffers;

	VkRenderPass				renderPass;
	VkPipelineLayout			pipelineLayout;
	VkPipeline					graphicsPipeline;


	VkCommandPool					commandPool;
	std::vector<VkCommandBuffer>	commandBuffers;

	std::vector<VkSemaphore>		imageAvailableSemaphores;
	std::vector<VkSemaphore>		renderFinishedSemaphores;
	std::vector<VkFence>			inFlightFences;
	uint32_t						currentFrame = 0;
};

