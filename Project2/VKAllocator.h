#pragma once

#ifndef VK_ALLOCATOR_H
#define VK_ALLOCATOR_H
#include <vulkan/vulkan.h>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <malloc.h>

//malloc memory function
static void* allocationFunction(void* userData, 
    size_t size, 
    size_t alignment,
    VkSystemAllocationScope allocationScope) {
    (void)userData;
    (void)alignment;
    (void)allocationScope;
    void* ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

//free memory funtion
static void freeFunction(void* userData, void* mem) {
    (void)userData;
    free(mem);
}

void* reallocationFunction(void* userData, 
    void* original, 
    size_t size, 
    size_t alignment,
    VkSystemAllocationScope allocationScope) {
    (void)userData;
    (void)alignment;
    (void)allocationScope;
    return realloc(original, size);
}

static void internalAllocationNotification(void* userData,
    size_t  size,
    VkInternalAllocationType   allocationType,
    VkSystemAllocationScope     allocationScope) {
    (void)userData;
    (void)size;
    (void)allocationType;
    (void)allocationScope;
}
static void internalFreeNotification(void* userData,
    size_t    size,
    VkInternalAllocationType   allocationType,
    VkSystemAllocationScope    allocationScope) {
    (void)userData;
    (void)size;
    (void)allocationType;
    (void)allocationScope;
}

class VKAllocator {
public:
    VKAllocator() {
    };
    ~VKAllocator() {
        if (allocationCallback)
            delete allocationCallback;
    }
public:
    VkAllocationCallbacks* getAllocator() {
        if (!allocationCallback)
            allocationCallback = new VkAllocationCallbacks();

        allocationCallback->pUserData = (void*)this;
        allocationCallback->pfnAllocation = (PFN_vkAllocationFunction)(&allocationFunction);
        allocationCallback->pfnReallocation = (PFN_vkReallocationFunction)(&reallocationFunction);
        allocationCallback->pfnFree = (PFN_vkFreeFunction)&freeFunction;
        allocationCallback->pfnInternalAllocation = (PFN_vkInternalAllocationNotification)
            &internalAllocationNotification;
        allocationCallback->pfnInternalFree = (PFN_vkInternalFreeNotification)(&internalFreeNotification);
        return allocationCallback;
    }
private:
    
private:
    VkAllocationCallbacks* allocationCallback = nullptr;
};

#endif // VK_ALLOCATOR_H