#pragma once
#ifndef VK_UNIFORMBUFFER_H
#define VK_UNIFORMBUFFER_H
#include <iostream>
#include <utility>
#include <vector>
#include <functional>
#include <vulkan/vulkan.h>
#include "VKApp.h"
#include "VMAllocator.h"
#include "Buffer.h"
class VKUniformBuffer {

public:
    VKUniformBuffer(VKApp* vkApp, uint32_t binding, uint32_t bufferSize) 
        :app(vkApp),bindingId(binding),bufferSize(bufferSize){
        bufferData.resize(bufferSize);
    }
    ~VKUniformBuffer(){}
public:
    void initBuffer(uint32_t swapImageChainSize) {
        clearBuffer();
        uniformBuffers.resize(swapImageChainSize);
        
        bufferInfos.resize(swapImageChainSize);

        for (size_t i = 0; i < swapImageChainSize; i++) {

            //context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            //    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i],
            //    uniformBuffersMemory[i]);

            VKBuffer* buffer = new VKBuffer(app, bufferSize, false,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
            uniformBuffers[i] = buffer;
            bufferInfos[i].buffer = uniformBuffers[i]->getBuffer();
            bufferInfos[i].offset = 0;
            bufferInfos[i].range = bufferSize;
        }

        needClear = true;
    }

    VkWriteDescriptorSet createWriteDescriptorSet(uint32_t index, VkDescriptorSet descriptorSet) {
        //
        assert(index < bufferInfos.size());
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSet;
        descriptorWrite.dstBinding = bindingId;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfos[index];
        return descriptorWrite;
    }

    void clearBuffer() {
        if (!uniformBuffers.empty()) {
            for (size_t i = 0; i < uniformBuffers.size(); i++)
            {
                uniformBuffers[i]->release();
            }
            uniformBuffers.clear();
        }
    }

    void release() {
        clearBuffer();
        bufferData.clear();
        bufferInfos.clear();
        delete this;
    }

    void setWriteDataCallback(std::function<uint32_t(char*&, uint32_t)> callback) {
        if (callback)
            writeDataCallback = callback;
    }

    void update(uint32_t index) {
        if (!writeDataCallback) {
            std::cerr << "please set write data callback function" << std::endl;
        }

        char* userData = bufferData.data();
        uint32_t size = writeDataCallback(userData, bufferData.size());
        if (size != bufferData.size()) {
            std::cerr << "write data callback size error" << std::endl;
            return;
        }

        void* gpuData;
        vmaMapMemory(app->getAllocator(), uniformBuffers[index]->getAllocation(), &gpuData);
        memcpy(gpuData, &userData, bufferSize);
        vmaUnmapMemory(app->getAllocator(), uniformBuffers[index]->getAllocation());
        return;
        //void* gpuData = nullptr;
        //vkMapMemory(context->getDevice(), uniformBuffersMemory[index], 0, bufferSize, 0, &gpuData);
        //memcpy(gpuData, userData, bufferSize);
        //vkUnmapMemory(context->getDevice(), uniformBuffersMemory[index]);
    }
protected:
    VKApp* app = nullptr;
    uint32_t bindingId = 0;
    uint32_t bufferSize = 0;

    std::function<uint32_t(char*&, uint32_t)> writeDataCallback;

    std::vector<char> bufferData;
    std::vector<VKBuffer*> uniformBuffers;
    //std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<VkDescriptorBufferInfo> bufferInfos;

    bool needClear = false;
};

#endif //VK_UNIFORMBUFFER_H