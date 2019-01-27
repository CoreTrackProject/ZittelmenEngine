#pragma once

#include <QVulkanInstance>
#include <QLoggingCategory>
#include <QWidget>
#include <iostream>

#include "Vulkan/VulkanWindow.h"
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanBase.h"

#include "Qt3D/Qt3DWindow.h"


class ZittelmenEngine
{

public:
    ZittelmenEngine();
	~ZittelmenEngine();


	// OLD
	Qt3DWindow* getQt3DWindow();


	VulkanWindow* getVulkanWindow();
	void setTargetRenderSurface(QWidget *targetRenderSurface);
	void initVulkanRenderer();

private:
	// I am a Velociraptor
	QVulkanInstance m_vulkanInstance;
	Qt3DWindow *renderer;


	VulkanBase *m_vulkanBase     = nullptr;
	QWidget *targetRenderSurface = nullptr;
	VulkanWindow *m_vulkanWindow = nullptr;
};
