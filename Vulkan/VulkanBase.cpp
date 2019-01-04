#include "VulkanBase.h"

#include <QVulkanInstance>
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>


#include <QVulkanFunctions>

VulkanBase::VulkanBase()
{
    this->initVulkan();
}



void VulkanBase::initVulkan()
{
    QVulkanInstance inst;
    inst.create();
    inst.setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");



}
