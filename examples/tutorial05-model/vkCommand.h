#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

class vkContext;


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

void createCommandPool(vkContext& contextref);
void createCommandBuffers(vkContext& contextref);
void recordCommandBuffer(vkContext& contextref, uint32_t imageIndex);



VkCommandBuffer beginSingleTimeCommands(vkContext& contextref);

void endSingleTimeCommands(vkContext& contextref, VkCommandBuffer commandBuffer);
void copyBuffer(vkContext& contextref, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
