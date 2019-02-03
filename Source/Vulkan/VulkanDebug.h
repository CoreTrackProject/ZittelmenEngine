#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

#include <QDebug>
#include <vector>
#include <algorithm>

class VulkanDebug
{

public:
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{

		qDebug() << "Debug utils data: " << messageType;

		return VK_FALSE;
	}

	static constexpr VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		VK_NULL_HANDLE,
		0,
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		VulkanDebug::debugCallback,
		nullptr
	};



public:
	VulkanDebug(VkInstance *instance);
	~VulkanDebug();

	static std::vector<const char*> addInstanceDebugExtensionCollection(std::vector<const char*> &extensionCollection);
	static std::vector<const char*> addInstanceDebugLayerCollection(std::vector<const char*> &extensionCollection);

private:
	void init_vulkanDebug();
	void destroy_vulkanDebug();


private:
	VkInstance *instance = nullptr;

	PFN_vkCreateDebugUtilsMessengerEXT  CreateDebugUtilsMessengerEXT = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT = nullptr;

	PFN_vkCreateDebugReportCallbackEXT  CreateDebugReportCallbackEXT = nullptr;
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallbackEXT = nullptr;

	
	VkDebugUtilsMessengerEXT debugMessenger;



};