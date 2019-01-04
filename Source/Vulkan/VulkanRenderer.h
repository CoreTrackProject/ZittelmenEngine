#pragma once

#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

class VulkanRenderer : public QVulkanWindowRenderer
{

public:
    VulkanRenderer(QVulkanWindow *w) : m_window(w) { }

    void initResources() override
    {
        m_devFuncs = m_window->vulkanInstance()->deviceFunctions(m_window->device());
    }

    //void initSwapChainResources() override { ... }
    //void releaseSwapChainResources() override { ... }
    //void releaseResources() override { ... }

    void startNextFrame() override
    {
        //VkCommandBuffer cmdBuf = m_window->currentCommandBuffer();

        //m_devFuncs->vkCmdBeginRenderPass(...);

        //m_window->frameReady();
    }

private:
    QVulkanWindow *m_window;
    QVulkanDeviceFunctions *m_devFuncs;
};

