#include "Platform/Vulkan/VKVertex.hpp"

using namespace Kestrel;

const std::array<vk::VertexInputAttributeDescription, 2> VK_Vertex3f3f::getAttributeDescription(){
	return {
		vk::VertexInputAttributeDescription( 0, 0, vk::Format::eR32G32B32Sfloat, offsetof( VK_Vertex3f3f, pos )),
		vk::VertexInputAttributeDescription( 1, 0, vk::Format::eR32G32B32Sfloat, offsetof( VK_Vertex3f3f, col ))
		};
}

const vk::VertexInputBindingDescription VK_Vertex3f3f::getBindingDescription(){
	return { 0, sizeof( VK_Vertex3f3f ), vk::VertexInputRate::eVertex };
}
