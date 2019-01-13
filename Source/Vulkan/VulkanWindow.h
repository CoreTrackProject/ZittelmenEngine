#pragma once


#include "VulkanRenderer.h"
#include <QVulkanWindow>

// This code is used from the qt vulkan examples


//Documentation QVulkanWindow:
// https://doc.qt.io/qt-5/qvulkanwindow.html#details

class VulkanWindow : public QVulkanWindow
{
	Q_OBJECT

public:
	VulkanWindow();
	QVulkanWindowRenderer *createRenderer() override;

};