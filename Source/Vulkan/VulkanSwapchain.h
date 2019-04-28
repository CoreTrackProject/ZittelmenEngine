#pragma once

#include <vector>
#include <algorithm>
#include <array>
#include <Vulkan/vulkan.h>

#include <QDebug>

#include "VulkanDevice.h"
#include "VulkanTexture.h"

/*

*/
struct VulkanSwapchainCreateInfo {
	VkPhysicalDevice device;
	VkDevice         logicalDevice; 
	DeviceInfo       deviceInfo; 
	VkSurfaceKHR     surface; 
	std::uint32_t    width; 
	std::uint32_t    height;
};


/*

*/
struct Image {
	VkImage image;
	VkImageView imageView;
};

/*

*/
class VulkanSwapchain
{
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	} details;
	

public:
	VulkanSwapchain(VulkanSwapchainCreateInfo createInfo);
	~VulkanSwapchain();

	VkExtent2D &GetSwapchainExtent2D();
	VkSurfaceFormatKHR &GetSwapchainImageFormat();
	VkSwapchainKHR &GetSwapchain();

	std::vector<Image> &GetImageCollection();
	uint32_t &getQueueFamilyPresentIdx();
	VkQueue &GetPresentQueue();

	VkRenderPass &GetRenderpass();

	std::vector<VkFramebuffer> GetFramebufferCollection();
	std::shared_ptr<VulkanTexture> GetDepthTexture();

private:
	void init_Swapchain();
	void querySwapChainRelatedInfo();
	void init_Imageviews();
	
	void init_DepthTexture();

	void init_Renderpass();

	void init_Framebuffer();


	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR   chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D         chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	

private:
	VulkanSwapchainCreateInfo createInfo = {};

	std::shared_ptr<VulkanTexture> depthTexture = nullptr;

	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	VkQueue presentQueue     = VK_NULL_HANDLE; 

	VkRenderPass renderPass  = VK_NULL_HANDLE;


	// Queuefamily index which supports presenting
	uint32_t queueFamilyPresentIdx = 0;

	VkSurfaceFormatKHR selectedSurfaceFormat;
	VkPresentModeKHR selectedPresentMode;
	VkExtent2D selectedExtent;

	Image image;
	std::vector<Image> imageCollection;

	std::vector<VkFramebuffer> swapchainFramebufferCollection;

};