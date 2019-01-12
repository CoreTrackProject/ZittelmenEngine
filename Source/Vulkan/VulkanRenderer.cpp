#include "VulkanRenderer.h"

// This code is used from the qt vulkan examples


VulkanRenderer::VulkanRenderer()
{
	this->initVkLayers();
	this->initVkInstance();
}

VulkanRenderer::~VulkanRenderer()
{

	
	vkDestroyInstance(this->m_vkInstance, VK_NULL_HANDLE);
}

void VulkanRenderer::initVkLayers()
{
	uint32_t instance_layer_count;
	VkLayerProperties *vk_props = VK_NULL_HANDLE;
	

	/*
	 * It's possible, though very rare, that the number of
	 * instance layers could change. For example, installing something
	 * could include new layers that the loader would pick up
	 * between the initial query for the count and the
	 * request for VkLayerProperties. The loader indicates that
	 * by returning a VK_INCOMPLETE status and will update the
	 * the count parameter.
	 * The count parameter will be updated with the number of
	 * entries loaded into the data pointer - in case the number	
	 * of layers went down or is smaller than the size given.
	 */
	VkResult res;
	do {

		res = vkEnumerateInstanceLayerProperties(&instance_layer_count, VK_NULL_HANDLE);
		
		if (res) {
			std::cout << "Failed to enumerate instance layer properties" << std::endl;
			exit(-1);
		}
		if (instance_layer_count == 0) {
			return;
			//return VK_SUCCESS;
		}

		vk_props = (VkLayerProperties *)realloc(vk_props, instance_layer_count * sizeof(VkLayerProperties));

		res = vkEnumerateInstanceLayerProperties(&instance_layer_count, vk_props);
	} while (res == VK_INCOMPLETE);


	/*
	 * Now gather the extension list for each instance layer.
	 */
	std::vector<VkLayerProperties> layerPropertyCollection(instance_layer_count);
	for (uint32_t i = 0; i < instance_layer_count; i++) {

		//layer_properties layer_props;
		//layer_props.properties = vk_props[i];
		/*res = init_global_extension_properties(layer_props);
		if (res) return res;
		info.instance_layer_properties.push_back(layer_props);*/
	}
	free(vk_props);

	//----------------------------------------------------

	/*VkExtensionProperties *instance_extensions;
	uint32_t instance_extension_count;
	VkResult res;
	char *layer_name = NULL;

	layer_name = layer_props.properties.layerName;

	do {
		res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, NULL);
		if (res) return res;

		if (instance_extension_count == 0) {
			return VK_SUCCESS;
		}

		layer_props.instance_extensions.resize(instance_extension_count);
		instance_extensions = layer_props.instance_extensions.data();
		res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, instance_extensions);
	} while (res == VK_INCOMPLETE);

	return res;*/

}

void VulkanRenderer::initVkInstance()
{
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = NULL;
	app_info.pApplicationName = "CoreTrack";
	app_info.applicationVersion = 1;
	app_info.pEngineName = "ZittelmenEngine";
	app_info.engineVersion = 1;
	app_info.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo inst_createinfo = {};

	inst_createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_createinfo.pNext = NULL;
	inst_createinfo.flags = 0;
	inst_createinfo.pApplicationInfo = &app_info;
	
	inst_createinfo.enabledExtensionCount = 0;
	inst_createinfo.ppEnabledExtensionNames = NULL;
	
	inst_createinfo.enabledLayerCount = 0;
	inst_createinfo.ppEnabledLayerNames = NULL;


	VkResult res = vkCreateInstance(&inst_createinfo, VK_NULL_HANDLE, &this->m_vkInstance);
	if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
		std::cout << "cannot find a compatible Vulkan ICD\n" << std::endl;
		exit(-1);

	} else if (res) {
		std::cout << "unknown error\n" << std::endl;
		exit(-1);
	}






}
