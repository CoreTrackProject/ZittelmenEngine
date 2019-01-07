#include "ZittelmenEngine.h"

ZittelmenEngine::ZittelmenEngine()
{
    this->m_vkWindow = new VulkanWindow();

    // Create Vulkan instance
    this->m_vkInstance.setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");

    if (!this->m_vkInstance.create())
        qFatal("Failed to create Vulkan instance: %d", this->m_vkInstance.errorCode());

    this->m_vkWindow->setVulkanInstance(&this->m_vkInstance);
}

VulkanWindow *ZittelmenEngine::getVulkanWindow()
{
    return this->m_vkWindow;
}
