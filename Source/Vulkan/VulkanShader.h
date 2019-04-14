#pragma once

#include <QDir>
#include <QStringLiteral>
#include <QDebug>

#include <vulkan/vulkan.h>

class VulkanShader {

public:
	VulkanShader(VkDevice &logicalDevice);
	~VulkanShader();

	VkShaderModule &getVertexShaderModule();
	VkShaderModule &getFragmentShaderModule();


private:
	void loadShaders();
	QByteArray loadFile(QString filePath);

private:
	VkDevice &logicalDevice;
	VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

};