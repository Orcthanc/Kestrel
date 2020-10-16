#pragma once

#include <kstpch.hpp>
#include <vulkan/vulkan.hpp>
#include <glm/vec3.hpp>

namespace Kestrel {

	struct VK_Vertex3f3f {
		static const std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescription();
		static const vk::VertexInputBindingDescription getBindingDescription();

		glm::vec3 pos;
		glm::vec3 col;

	};
}
