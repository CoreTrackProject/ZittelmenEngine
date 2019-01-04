#pragma once

// Implemetation based on tutorial:
// https://vulkan-tutorial.com

#include <vulkan/vulkan.h>

class VulkanBase
{
public:
    VulkanBase();

private:
    void initVulkan();


private: // Memer variables
    VkInstance m_vkInstance;


};
