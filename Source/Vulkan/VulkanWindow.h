#pragma once

#include <QVulkanInstance>
#include <QVulkanWindow>

#include "VulkanRenderer.h"


// This code is used from the qt vulkan examples


//Documentation QVulkanWindow:
// https://doc.qt.io/qt-5/qvulkanwindow.html#details

class VulkanWindow : public QVulkanWindow
{

public:
    VulkanWindow();
    QVulkanWindowRenderer *createRenderer() override;

};
