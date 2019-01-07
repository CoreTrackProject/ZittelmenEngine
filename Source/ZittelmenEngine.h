#pragma once

#include <QVulkanInstance>
#include <iostream>

#include "zittelmenengine_global.h"
#include "Vulkan/VulkanWindow.h"



class ZITTELMENENGINESHARED_EXPORT ZittelmenEngine
{

public:
    ZittelmenEngine();

    VulkanWindow* getVulkanWindow();

private:
    QVulkanInstance m_vkInstance;
    VulkanWindow *m_vkWindow = nullptr;

};
