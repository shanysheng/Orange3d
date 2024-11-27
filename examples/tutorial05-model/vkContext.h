#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <chrono>

#include "vkInstance.h"
#include "vkLayer.h"
#include "vkDevice.h"
#include "vkSwapchain.h"
#include "vkGraphicsPipeline.h"
#include "vkCommand.h"
#include "vkTexture.h"


#include "objModel.h"

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

// VkPipelineLayout��VkPipeline�Ĺ��ܲ���
// VkDescriptorSetLayout�� VkDescriptorPool��VkDescriptorSet���ܲ���



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

	VkImage							depthImage;
	VkDeviceMemory					depthImageMemory;
	VkImageView						depthImageView;

	VkImage							textureImage;
	VkDeviceMemory					textureImageMemory;
	VkImageView						textureImageView;
	VkSampler						textureSampler;

	VkBuffer						vertexBuffer;
	VkDeviceMemory					vertexBufferMemory;
	VkBuffer						indexBuffer;
	VkDeviceMemory					indexBufferMemory;

	std::vector<VkBuffer>			uniformBuffers;
	std::vector<VkDeviceMemory>		uniformBuffersMemory;
	std::vector<void*>				uniformBuffersMapped;

	VkDescriptorSetLayout			descriptorSetLayout;
	VkDescriptorPool				descriptorPool;
	std::vector<VkDescriptorSet>	descriptorSets;

	objModel						modelData;

	void initContext() {
		createInstance(*this);
		setupDebugMessenger(this->instance);
		createSurface(*this);

		pickPhysicalDevice(*this);
		createLogicalDevice(*this);
		createSwapChain(*this);
		createImageViews(*this);
		createRenderPass(*this);
		createDescriptorSetLayout();
		createGraphicsPipeline(*this);

		createCommandPool(*this);
		createDepthResources(*this);
		createFramebuffers(*this);

		createTextureImage(*this, TEXTURE_PATH);
		createTextureImageView(*this);
		createTextureSampler(*this);

		loadModel(this->modelData);

		createVertexBuffer(*this, this->modelData.vertices.data(), this->modelData.vertices.size());
		createIndexBuffer(*this, this->modelData.indices.data(), this->modelData.indices.size());
		createUniformBuffers(*this, sizeof(UniformBufferObject));
		createDescriptorPool(*this);
		createDescriptorSets(*this, sizeof(UniformBufferObject));

		createCommandBuffers(*this);
		createSyncObjects(*this);
	}

	void createSurface(vkContext& contextref) {
		if (glfwCreateWindowSurface(contextref.instance, contextref.window, nullptr, &contextref.surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}



	void createSyncObjects(vkContext& contextref) {
		contextref.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		contextref.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		contextref.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(contextref.logicaldevice, &semaphoreInfo, nullptr, &contextref.imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(contextref.logicaldevice, &semaphoreInfo, nullptr, &contextref.renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(contextref.logicaldevice, &fenceInfo, nullptr, &contextref.inFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}


	void createDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(logicaldevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void updateUniformBuffer(uint32_t currentImage) {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}

	void cleanupSwapChain() {
		vkDestroyImageView(logicaldevice, depthImageView, nullptr);
		vkDestroyImage(logicaldevice, depthImage, nullptr);
		vkFreeMemory(logicaldevice, depthImageMemory, nullptr);

		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(logicaldevice, framebuffer, nullptr);
		}

		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(logicaldevice, imageView, nullptr);
		}

		vkDestroySwapchainKHR(logicaldevice, swapChain, nullptr);
	}

	void cleanContext() {
		cleanupSwapChain();

		vkDestroyPipeline(logicaldevice, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(logicaldevice, pipelineLayout, nullptr);

		vkDestroyRenderPass(logicaldevice, renderPass, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyBuffer(logicaldevice, uniformBuffers[i], nullptr);
			vkFreeMemory(logicaldevice, uniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(logicaldevice, descriptorPool, nullptr);

		vkDestroySampler(logicaldevice, textureSampler, nullptr);
		vkDestroyImageView(logicaldevice, textureImageView, nullptr);

		vkDestroyImage(logicaldevice, textureImage, nullptr);
		vkFreeMemory(logicaldevice, textureImageMemory, nullptr);

		vkDestroyDescriptorSetLayout(logicaldevice, descriptorSetLayout, nullptr);

		vkDestroyBuffer(logicaldevice, indexBuffer, nullptr);
		vkFreeMemory(logicaldevice, indexBufferMemory, nullptr);

		vkDestroyBuffer(logicaldevice, vertexBuffer, nullptr);
		vkFreeMemory(logicaldevice, vertexBufferMemory, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(logicaldevice, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(logicaldevice, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(logicaldevice, inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(logicaldevice, commandPool, nullptr);

		vkDestroyDevice(logicaldevice, nullptr);

		cleanupDebugMessenger(instance);

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

	}



};

