include(Shaders/Shaders.pri)

SOURCES += \
    $${PWD}/VulkanBase.cpp \
    $${PWD}/VulkanWindow.cpp \
    $${PWD}/VulkanRenderer.cpp

HEADERS += \
    $${PWD}/VulkanBase.h \
    $${PWD}/VulkanWindow.h \
    $${PWD}/VulkanRenderer.h