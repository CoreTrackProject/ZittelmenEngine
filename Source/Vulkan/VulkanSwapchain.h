#pragma once

#include <vector>
#include <algorithm>
#include <Vulkan/vulkan.h>

#include "VulkanDevice.h"

struct Image {
	VkImage image;
	VkImageView imageView;
};

class VulkanSwapchain
{
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	} details;
	

public:
	VulkanSwapchain(VkPhysicalDevice *device, VkDevice* logicalDevice, DeviceInfo* deviceInfo, VkSurfaceKHR *surface);
	~VulkanSwapchain();

	VkExtent2D* getSwapchainExtent2D();
	VkSurfaceFormatKHR* getSwapchainImageFormat();
	std::vector<Image>* getImageCollection();

private:
	void init_Swapchain();
	void querySwapChainRelatedInfo();
	void init_Imageviews();

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	

private:
	VkSurfaceKHR *surface = VK_NULL_HANDLE;
	VkPhysicalDevice *device = VK_NULL_HANDLE; 
	VkDevice* logicalDevice = VK_NULL_HANDLE;
	DeviceInfo* deviceInfo = nullptr;
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;


	VkSurfaceFormatKHR selectedSurfaceFormat;
	VkPresentModeKHR selectedPresentMode;
	VkExtent2D selectedExtent;

	Image image;
	std::vector<Image> imageCollection;
	
};