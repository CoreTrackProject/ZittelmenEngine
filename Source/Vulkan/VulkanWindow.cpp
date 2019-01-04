#include "VulkanWindow.h"

VulkanWindow::VulkanWindow()
{
    QVulkanInstance instance;

    // Create Vulkan instance
    instance.setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");

    if (!instance.create())
        qFatal("Failed to create Vulkan instance: %d", instance.errorCode());

    this->setVulkanInstance(&instance);

}

QVulkanWindowRenderer *VulkanWindow::createRenderer()
{
    return new VulkanRenderer(this);
}
