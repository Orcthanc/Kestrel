#include "Platform/Vulkan/VKVertex.hpp"

using namespace Kestrel;

const std::array<vk::VertexInputAttributeDescription, 3> VK_Vertex::getAttributeDescription(){
	return {
			vk::VertexInputAttributeDescription( 0, 0, vk::Format::eR32G32B32Sfloat, offsetof( VK_Vertex, pos )),
			vk::VertexInputAttributeDescription( 1, 0, vk::Format::eR32G32B32Sfloat, offsetof( VK_Vertex, col )),
			vk::VertexInputAttributeDescription( 2, 0, vk::Format::eR32G32Sfloat, offsetof( VK_Vertex, tex ))
		};
}

const vk::VertexInputBindingDescription VK_Vertex::getBindingDescription(){
	return { 0, sizeof( VK_Vertex ), vk::VertexInputRate::eVertex };
}
