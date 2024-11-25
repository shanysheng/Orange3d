#include "vkVertexBuffer.h"
#include "vkContext.h"


uint32_t findMemoryType(vkContext& contextref, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(contextref.physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}


void createBuffer(vkContext& contextref, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(contextref.logicaldevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(contextref.logicaldevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(contextref, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(contextref.logicaldevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(contextref.logicaldevice, buffer, bufferMemory, 0);
}

void copyBuffer(vkContext& contextref, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = contextref.commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(contextref.logicaldevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(contextref.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(contextref.graphicsQueue);

    vkFreeCommandBuffers(contextref.logicaldevice, contextref.commandPool, 1, &commandBuffer);
}

void createVertexBuffer(vkContext& contextref, const Vertex* pvertex, size_t vertexCount) {
    VkDeviceSize bufferSize = sizeof(pvertex[0]) * vertexCount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(contextref, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(contextref.logicaldevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, pvertex, (size_t)bufferSize);
    vkUnmapMemory(contextref.logicaldevice, stagingBufferMemory);

    createBuffer(contextref, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, contextref.vertexBuffer, contextref.vertexBufferMemory);

    copyBuffer(contextref, stagingBuffer, contextref.vertexBuffer, bufferSize);

    vkDestroyBuffer(contextref.logicaldevice, stagingBuffer, nullptr);
    vkFreeMemory(contextref.logicaldevice, stagingBufferMemory, nullptr);
}

void createIndexBuffer(vkContext& contextref, const uint16_t* pindices, size_t indexCount) {
    VkDeviceSize bufferSize = sizeof(pindices[0]) * indexCount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(contextref, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(contextref.logicaldevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, pindices, (size_t)bufferSize);
    vkUnmapMemory(contextref.logicaldevice, stagingBufferMemory);

    createBuffer(contextref, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, contextref.indexBuffer, contextref.indexBufferMemory);

    copyBuffer(contextref, stagingBuffer, contextref.indexBuffer, bufferSize);

    vkDestroyBuffer(contextref.logicaldevice, stagingBuffer, nullptr);
    vkFreeMemory(contextref.logicaldevice, stagingBufferMemory, nullptr);
}

void createUniformBuffers(vkContext& contextref, VkDeviceSize bufferSize) {

    contextref.uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    contextref.uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    contextref.uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(contextref, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, contextref.uniformBuffers[i], contextref.uniformBuffersMemory[i]);

        vkMapMemory(contextref.logicaldevice, contextref.uniformBuffersMemory[i], 0, bufferSize, 0, &contextref.uniformBuffersMapped[i]);
    }
}

void createDescriptorPool(vkContext& contextref) {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(contextref.logicaldevice, &poolInfo, nullptr, &contextref.descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void createDescriptorSets(vkContext& contextref, VkDeviceSize bufferSize) {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, contextref.descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = contextref.descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    contextref.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(contextref.logicaldevice, &allocInfo, contextref.descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = contextref.uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = bufferSize;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = contextref.descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(contextref.logicaldevice, 1, &descriptorWrite, 0, nullptr);
    }
}
