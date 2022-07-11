#pragma once
#ifndef VK_BUFFER_H
#define VK_BUFFER_H
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VKApp.h"
#include "PoolBase.h"

class VKBuffer {
public:
	VKBuffer() = delete;
	VKBuffer(VKApp* vkApp, VkDeviceSize size, bool isupload,
		VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VmaMemoryUsage memusage = VMA_MEMORY_USAGE_AUTO) :app(vkApp) {

		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = size;
		bufferInfo.usage = usage;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = memusage;
		if (isupload) {
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}

		//VkBuffer buffer;
		//VmaAllocation allocation;
		vmaCreateBuffer(app->getAllocator(), &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
	}

	~VKBuffer() {

	}
public:
	void release() {
		if (buffer) {
			vmaDestroyBuffer(app->getAllocator(), buffer, allocation);
		}
		buffer = nullptr;
		delete this;
	}
	VKApp* getApp() {
		return app;
	}
	VkBuffer getBuffer() {
		return buffer;
	}
	VmaAllocation getAllocation() {
		return allocation;
	}
	void setAllocation(VmaAllocation& alloc) {
		if (alloc) {
			allocation = alloc;
		}
	}
	void copyBufferFrom(VkBuffer& srcBuffer, VkDeviceSize size) {
		VkCommandBuffer commandBuffer = app->getCmdPool()->beginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, buffer, 1, &copyRegion);

		app->getCmdPool()->endSingleTimeCommands(commandBuffer, app->getDevice()->getGraphicsQueue());
	}
	
private:
	VKApp* app = nullptr;
	VkBuffer buffer;
	VmaAllocation allocation;
};



class BufferBase {
public:
	BufferBase(VKApp* vkApp) :app(vkApp) {

	}
	virtual ~BufferBase() {

	}
public:
	virtual void release() {
		if (buffer) {
			buffer->release();
			buffer = nullptr;
		}
		delete  this;
	}
	VkBuffer getBuffer() {
		return buffer->getBuffer();
	}
	VmaAllocation getVmaAllocation() {
		return buffer->getAllocation();
	}
	virtual void render(VkCommandBuffer command) = 0;
public:
	VKApp* app = nullptr;
	VKBuffer* buffer = nullptr;
};

#endif // VK_BUFFER_H