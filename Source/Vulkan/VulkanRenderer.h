#pragma once

#include <QVulkanWindow>
#include <QVulkanFunctions>

// This code is used from the qt vulkan examples

class VulkanRenderer : public QVulkanWindowRenderer
{

public:
	VulkanRenderer(QVulkanWindow *w);

	void initResources() override;
	void initSwapChainResources() override;
	void releaseSwapChainResources() override;
	void releaseResources() override;
	void startNextFrame() override;

private:
	QVulkanWindow *m_window;
	QVulkanDeviceFunctions *m_devFuncs;
	float m_green = 0;

};

