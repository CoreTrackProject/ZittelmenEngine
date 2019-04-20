#pragma once

#include <iostream>
#include "Vulkan\VulkanInstance.h"


class MemoryLeakTest {

	// Test Vulkan Instance class
public:
	MemoryLeakTest() {
		this->testVulkanInstanceClass();
	}

	~MemoryLeakTest() {

	}


	void testVulkanInstanceClass() {
		
		std::unique_ptr<VulkanInstance> instance = nullptr;
		std::unique_ptr<VulkanDebug> vulkanDebug = nullptr;
		std::unique_ptr<VulkanDevice> vulkanDevice = nullptr;

		for (int i = 0; i < 1000; i++) {
			instance.reset(new VulkanInstance(true));
			vulkanDebug.reset(new VulkanDebug(instance->GetInstance()));
			vulkanDevice.reset(new VulkanDevice(instance->GetInstance()));

			vulkanDevice.reset();
			vulkanDebug.reset();
			instance.reset();
		}

		std::cout << "Finished" << std::endl;
		
	}

};