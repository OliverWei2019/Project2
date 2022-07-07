#pragma once
#ifndef VK_SUBPASS_H
#define VK_SUBPASS_H
#include <vector>
#include <vulkan/vulkan.h>
#include "VKApp.h"
#include "VMAllocator.h"
#include "VKUtil.h"

class subpassSet {
public:
	subpassSet(VKApp* vkApp) :app(vkApp) {

	}
	~subpassSet() {

	}
public:
	void addAttachRef(attachmentsRef *attachsRef) {
		attachsRefSet.push_back(attachsRef);
	}
	void generateSubpassDescription() {
		subpassDescriptions.resize(attachsRefSet.size());
		for (size_t i = 0; i < attachsRefSet.size(); i++) {
			subpassDescriptions[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescriptions[i].colorAttachmentCount = static_cast<uint32_t>(attachsRefSet[i]->colorAttachRef.size());
			subpassDescriptions[i].pColorAttachments = attachsRefSet[i]->colorAttachRef.data();
			subpassDescriptions[i].pDepthStencilAttachment = attachsRefSet[i]->depthAttachRef.data();
			subpassDescriptions[i].inputAttachmentCount = static_cast<uint32_t>(attachsRefSet[i]->inputAttachRef.size());
			subpassDescriptions[i].pInputAttachments = attachsRefSet[i]->inputAttachRef.data();
		}
	}
	void release() {
		if (!attachsRefSet.empty()) {
			for (auto attachRef : attachsRefSet) {
				attachRef->colorAttachRef.clear();
				attachRef->depthAttachRef.clear();
				attachRef->inputAttachRef.clear();
			}
			attachsRefSet.clear();
		}
		if (!subpassDescriptions.empty()) {
			subpassDescriptions.clear();
		}
		delete this;
	}
	uint32_t getSubpassCount() {
		return subpassDescriptions.size();
	}
	VkSubpassDescription* getSubpassDescriptionData() {
		return subpassDescriptions.data();
	}
private:
	VKApp* app = nullptr;
	std::vector<attachmentsRef*> attachsRefSet;
	std::vector<VkSubpassDescription> subpassDescriptions;
};
#endif // !VK_SUBPASS_H
