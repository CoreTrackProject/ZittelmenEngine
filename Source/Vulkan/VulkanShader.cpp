#include "VulkanShader.h"

VulkanShader::VulkanShader(VkDevice &logicalDevice) : logicalDevice(logicalDevice){
	Q_INIT_RESOURCE(CompiledShaders);
	

	this->loadShaders();
}

VulkanShader::~VulkanShader() {
	Q_CLEANUP_RESOURCE(CompiledShaders);

	vkDestroyShaderModule(this->logicalDevice, this->fragmentShaderModule, nullptr);
	vkDestroyShaderModule(this->logicalDevice, this->vertexShaderModule,    nullptr);

}

VkShaderModule &VulkanShader::getVertexShaderModule()
{
	return this->vertexShaderModule;
}

VkShaderModule &VulkanShader::getFragmentShaderModule()
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
	VkResult res = vkCreateShaderModule(this->logicalDevice, &vertexShaderInfo, nullptr, &this->vertexShaderModule);

	VkShaderModuleCreateInfo fragmentShaderInfo = {};
	fragmentShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragmentShaderInfo.codeSize = fragmentShader.size();
	fragmentShaderInfo.pCode = reinterpret_cast<const uint32_t *>(fragmentShader.constData());
	res = vkCreateShaderModule(this->logicalDevice, &fragmentShaderInfo, nullptr, &this->fragmentShaderModule);

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
