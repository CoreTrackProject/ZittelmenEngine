#pragma once

#include <QVulkanInstance>
#include <QVulkanWindow>

#include "VulkanRenderer.h"

//Documentation QVulkanWindow:
// https://doc.qt.io/qt-5/qvulkanwindow.html#details

class VulkanWindow : public QVulkanWindow
{
public:
    VulkanWindow();
    QVulkanWindowRenderer *createRenderer() override;


private:

};
