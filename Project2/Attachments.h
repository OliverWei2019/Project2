#pragma once
#ifndef  VK_ATTACHMENT_H
#define  VK_ATTACHMENT_H
#include <vector>
#include <vulkan/vulkan.h>
#include "VKApp.h"
#include "VMAllocator.h"
//#include "VKUtil.h"
// G-Buffer framebuffer attachments
struct FrameBufferAttachment {
	VKImages* image = VK_NULL_HANDLE;
	VKImageView* imageVIew = VK_NULL_HANDLE;
	VkFormat format;
};
struct Attachments {
	FrameBufferAttachment* position = VK_NULL_HANDLE;
	FrameBufferAttachment* normal = VK_NULL_HANDLE;
	FrameBufferAttachment* albedo = VK_NULL_HANDLE;
	int32_t width;
	int32_t height;
};
struct attachmentsRef {
	std::vector< VkAttachmentReference> colorAttachRef;
	std::vector< VkAttachmentReference> depthAttachRef;
	std::vector< VkAttachmentReference> inputAttachRef;
};
class AttachmentSet {
public:
	AttachmentSet(VKApp* vkApp) :app(vkApp) {

	}
	~AttachmentSet() {

	}
public:
	void createFrameBufferAttach(VkFormat format, VkImageUsageFlags usage, FrameBufferAttachment* attachment) {
		if (attachment->image->getImage() != VK_NULL_HANDLE) {
			clearFrameBufferAttach(attachment);
		}

		VkImageAspectFlags aspectMask = 0;
		VkImageLayout imageLayout;

		attachment->format = format;

		if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		{
			aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		assert(aspectMask > 0);
		
		VkExtent2D extent = app->getSwapChain()->getSwapChainExtent();
		attachment->image = new VKImages(app);
		attachment->image->createImage(extent.width, extent.height,
			VK_SAMPLE_COUNT_1_BIT, 1, format, VK_IMAGE_TILING_OPTIMAL,
			usage | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);

		VkImageViewCreateInfo viewInfo = initializers::createImageViewCreateInfo();
		viewInfo.format = format;
		viewInfo.image = attachment->image->getImage();
		viewInfo.subresourceRange.aspectMask = aspectMask;
		attachment->imageVIew = new VKImageView(app);
		attachment->imageVIew->createImageView(viewInfo);
		attachment->imageVIew->setLayout(imageLayout);
	}
	void clearFrameBufferAttach(FrameBufferAttachment* attachment) {
		if(attachment->image)
			attachment->image->release();
		attachment->image == nullptr;
		if(attachment->imageVIew)
			attachment->imageVIew->release();
		attachment->imageVIew = nullptr;
	}
	void creatGbufferAttachments() {
		//world space positions
		createFrameBufferAttach(VK_FORMAT_R16G16B16A16_SFLOAT, 
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
			(GbufferAttachments.position));
		//world space normals
		createFrameBufferAttach(VK_FORMAT_R16G16B16A16_SFLOAT, 
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
			GbufferAttachments.normal);
		//albedo color
		createFrameBufferAttach(VK_FORMAT_R8G8B8A8_UNORM, 
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
			GbufferAttachments.albedo);
		VkExtent2D extent = app->getSwapChain()->getSwapChainExtent();
		GbufferAttachments.width = extent.width;
		GbufferAttachments.height = extent.height;
	}
	void destroyGbufferAttachments() {
		if (GbufferAttachments.position) {
			clearFrameBufferAttach(GbufferAttachments.position);
			GbufferAttachments.position = nullptr;
		}
		if (GbufferAttachments.normal) {
			clearFrameBufferAttach(GbufferAttachments.normal);
			GbufferAttachments.normal = nullptr;
		}
		if (GbufferAttachments.albedo) {
			clearFrameBufferAttach(GbufferAttachments.albedo);
			GbufferAttachments.albedo = nullptr;
		}
	}
	void createAttachments() {
		uint32_t framesCount = app->getSwapChain()->getSwapChainSize();
		VkExtent2D extent = app->getSwapChain()->getSwapChainExtent();
		AttachsColor.resize(framesCount);
		AttachsNormal.resize(framesCount);
		AttachsDepth.resize(framesCount);
		VkFormat depthFormat = app->getDevice()->findSupportImageFormat({ VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		for (uint32_t i = 0; i < framesCount; i++) {
			createFrameBufferAttach(VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				AttachsColor[i]);
			createFrameBufferAttach(VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				AttachsNormal[i]);
			createFrameBufferAttach(depthFormat,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				AttachsDepth[i]);
		}
	}
	void destroyAttachments() {
		if (!AttachsColor.empty()) {
			for (size_t i = 0; i < AttachsColor.size(); i++) {
				clearFrameBufferAttach(AttachsColor[i]);
			}
			AttachsColor.clear();
		}
		if (!AttachsNormal.empty()) {
			for (size_t i = 0; i < AttachsNormal.size(); i++) {
				clearFrameBufferAttach(AttachsNormal[i]);
			}
			AttachsNormal.clear();
		}
		if (!AttachsDepth.empty()) {
			for (size_t i = 0; i < AttachsDepth.size(); i++) {
				clearFrameBufferAttach(AttachsDepth[i]);
			}
			AttachsDepth.clear();
		}
	}
	
	void generateAttachmentsDescriptionSet() {
		VkAttachmentDescription attachsDescription = initializers::createAttachsDescription();
		//swap chain attachment
		attachsDescription.format = app->getSwapChain()->getSwapChainImageFormat();
		attachmentDescriptionSet.push_back(attachsDescription);

		/*
		* Gbuffer Attachments
		*/

		//Depth attachment
		VkFormat depthFormat = app->getDevice()->findSupportImageFormat({ VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		attachsDescription = initializers::createAttachsDescription();
		attachsDescription.format = depthFormat;
		attachsDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachsDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachmentDescriptionSet.push_back(attachsDescription);

	}
	void destroyAttachmentsDescriptionSet() {
		if (!attachmentDescriptionSet.empty()) {
			attachmentDescriptionSet.clear();
		}
	}
	void release() {
		destroyGbufferAttachments();
		destroyAttachments();
		destroyAttachmentsDescriptionSet();
		delete this;
	}
public:
	uint32_t getAttachmentsCount() {
		return attachmentDescriptionSet.size();
	}
	VkAttachmentDescription* getAttachmentsDescriptionData() {
		return attachmentDescriptionSet.data();
	}
	FrameBufferAttachment* getColorAttachs(uint32_t index) {
		if (AttachsColor.empty()) {
			std::cerr << " color attachments vector is empty!" << std::endl;
			return nullptr;
		}
		if (index >= AttachsColor.size()) {
			std::cerr << "index over color attachments vector size!" << std::endl;
			return nullptr;
		}
		return AttachsColor[index];
	}
	FrameBufferAttachment* getDepthAttachs(uint32_t index) {
		if (AttachsDepth.empty()) {
			std::cerr << " Depth attachments vector is empty!" << std::endl;
			return nullptr;
		}
		if (index >= AttachsDepth.size()) {
			std::cerr << "index over depth attachments vector size!" << std::endl;
			return nullptr;
		}
		return AttachsDepth[index];
	}
private:
	VKApp* app = nullptr;
	Attachments GbufferAttachments;
	std::vector<FrameBufferAttachment*> AttachsColor;
	std::vector<FrameBufferAttachment*> AttachsNormal;
	std::vector<FrameBufferAttachment*> AttachsDepth;

	std::vector<VkAttachmentDescription> attachmentDescriptionSet;


};
#endif // ! VK_ATTACHMENT_H
