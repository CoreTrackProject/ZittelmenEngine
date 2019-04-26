#pragma once

#include <QDir>
#include <QStringLiteral>
#include <QDebug>

#include <vulkan/vulkan.h>

/*

*/
struct VulkanShaderCreateInfo {
	VkDevice logicalDevice;
};

/*

*/
class VulkanShader {

public:
	VulkanShader(VulkanShaderCreateInfo createInfo);
	~VulkanShader();

	VkShaderModule &GetVertexShaderModule();
	VkShaderModule &GetFragmentShaderModule();


private:
	void loadShaders();
	QByteArray loadFile(QString filePath);

private:
	VulkanShaderCreateInfo createInfo = {};

	VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

};