#ifndef _VK_APP_H
#define _VK_APP_H
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include "VKAppConfig.h"
#include "VKAllocator.h"
#include "VMAllocator.h"

class VKValidationLayer;
class VKDevices;
class VKCmdPool;
class VKPipelineCache;
class VKRenderPass;
class VKSwapChain;
class Pipeline;
class VKSampler;

class VKApp {

public:
    virtual bool initWindow()  = 0;
    virtual void initVulkan(const Config config)  = 0;
    virtual void createInstance()  = 0;
    virtual void createSurface()  = 0;


public:
    virtual VmaAllocator getAllocator() = 0;
    virtual GLFWwindow* getWindow() const = 0;
    virtual VkInstance getInstance() const = 0;
    virtual VKValidationLayer* getValidationLayer() const = 0;
    virtual VkSurfaceKHR getSurface() const = 0;
    virtual VKDevices* getDevice() const = 0;
    virtual VKSwapChain* getSwapChain() const = 0;
    virtual VKPipelineCache* getPipelineCache() const = 0;
    virtual VKCmdPool* getCmdPool() const = 0;
    virtual VKRenderPass* getRenderPass() const = 0;
    virtual Pipeline* getPipeline(uint32_t index) const = 0;
    virtual void addPipeline(Pipeline*) = 0;
    virtual void removeSampler(VKSampler* sampler) = 0;
};


#endif// _VK_APP_H