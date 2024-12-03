#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>

#include "kBuffer.h"
#include "kSwapchain.h"
#include "kUniformBuffer.h"
#include "kRenderpaas.h"
#include "kContext.h"
#include "kGraphicPipeline.h"


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


// ��Ⱦ�������첽�ģ���ȡswap chain image��ִ��command buffer�����ѽ��д��swap chain image��д����ɺ���Խ���present
// ������ڼ������⣺
// 1����ȡswap chain image���ܻ���ʹ�ã���Ҫ�ȴ���ʹ���ꣻ
// 2��ִ��command buffer��ִ������Ҫ��֪���������̣������д�룻
// 3��presnet���̵�ʱ����Ҫ�ȴ�����֪swap chain image�Ѿ�д����ɣ����Խ��г��֣�

// ���Էֳ��������̣���Ⱦ���̺ͳ������̣�
// ��Ⱦ������Ҫ����VkSemaphore��
// 1���ȴ�swap chain image��ȡ�ɹ�������д���imageAvailableSemaphore
// 2��ִ��command buffer����Ⱦ���д��swap chain image��ɣ���֪�������̵�renderFinishedSemaphore
// ����������Ҫһ��VkSemaphore��
// 1��swap chain image��Ⱦ���д����ɣ��ɱ�����renderFinishedSemaphore

// �������̣�
// ��ȡ���õ�swap chain image index��
// reset command buffer
// ��ʼ��¼command buffer��ָ��render pass��pipeline��image indexָ����framebuffer��
// ����VkSubmitInfo��ָ��imageAvailableSemaphore��renderFinishedSemaphore���ύִ��vkQueueSubmit
// 
// �������̣�
// ����VkPresentInfoKHR��ָ��renderFinishedSemaphore��vkQueuePresentKHR


const int MAX_FRAMES_IN_FLIGHT = 2;


class kEngine
{
public:
	kEngine() {
		m_pWindow = nullptr;
		currentFrame = 0;
	}

	virtual ~kEngine() {
	}

	void createEngine(GLFWwindow* pwindow);
	void drawFrame();
	void cleanEngine();

	void frameChanged() { framebufferResized = true; }

protected:

	void recreateSwapChain();

	void recordCommandBuffer(uint32_t imageIndex);

	void createSyncObjects();
	void createCommandBuffers();
	void createDescriptorSets(VkDeviceSize bufferSize);
	void createUniformBuffers(VkDeviceSize bufferSize);
	void updateUniformBuffer(uint32_t currentImage);

	VkExtent2D chooseSwapExtent(GLFWwindow * pwindow, const VkSurfaceCapabilitiesKHR & capabilities);

protected:

	GLFWwindow*						m_pWindow;

	kContext						m_Context;

	kGraphicPipeline				m_GraphicPipeline;

	kSwapchain						m_Swapchain;
	kRenderpaas						m_Renderpass;

	Model							m_Model;

	std::vector<VkDescriptorSet>	descriptorSets;
	std::vector<VkCommandBuffer>	commandBuffers;
	std::vector<kUniformBuffer*>	m_UniformBuffers;

	std::vector<VkSemaphore>		imageAvailableSemaphores;
	std::vector<VkSemaphore>		renderFinishedSemaphores;
	std::vector<VkFence>			inFlightFences;
	uint32_t						currentFrame = 0;

	bool							framebufferResized = false;

};

