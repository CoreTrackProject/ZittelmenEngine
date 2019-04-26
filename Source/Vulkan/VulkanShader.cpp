#include "VulkanShader.h"

VulkanShader::VulkanShader(VulkanShaderCreateInfo createInfo) : createInfo(createInfo) {
	Q_INIT_RESOURCE(CompiledShaders);
	
	this->loadShaders();
}

VulkanShader::~VulkanShader() {
	Q_CLEANUP_RESOURCE(CompiledShaders);

	vkDestroyShaderModule(this->createInfo.logicalDevice, this->fragmentShaderModule,  nullptr);
	vkDestroyShaderModule(this->createInfo.logicalDevice, this->vertexShaderModule,    nullptr);

}


VkShaderModule &VulkanShader::GetVertexShaderModule()
{
	return this->vertexShaderModule;
}

VkShaderModule &VulkanShader::GetFragmentShaderModule()
{
	return this->fragmentShaderModule;
}


void VulkanShader::loadShaders()
{
	const QString &vertex_shader   = QStringLiteral(":/color_vert.spv");
	const QString &fragment_shader = QStringLiteral(":/color_frag.spv");

	QByteArray vertexShader = this->loadFile(vertex_shader);
	QByteArray fragmentShader = this->loadFile(fragment_shader);

	VkShaderModuleCreateInfo vertexShaderInfo = {};
	vertexShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertexShaderInfo.codeSize = vertexShader.size();
	vertexShaderInfo.pCode = reinterpret_cast<const uint32_t *>(vertexShader.constData());
	VkResult res = vkCreateShaderModule(this->createInfo.logicalDevice, &vertexShaderInfo, nullptr, &this->vertexShaderModule);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to create vertex shader module.");
	}

	VkShaderModuleCreateInfo fragmentShaderInfo = {};
	fragmentShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragmentShaderInfo.codeSize = fragmentShader.size();
	fragmentShaderInfo.pCode = reinterpret_cast<const uint32_t *>(fragmentShader.constData());
	res = vkCreateShaderModule(this->createInfo.logicalDevice, &fragmentShaderInfo, nullptr, &this->fragmentShaderModule);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to create fragment shader module.");
	}
}

QByteArray VulkanShader::loadFile(QString filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		qWarning("Failed to read shader %s", qPrintable(filePath));
	}
	QByteArray blob = file.readAll();

	file.close();

	return blob;
}
