#pragma once

#include <kstpch.hpp>
#include <vulkan/vulkan.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Kestrel {

	struct VK_ViewProj {
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 viewproj;
	};

	struct VK_Vertex {
		static const std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescription();
		static const vk::VertexInputBindingDescription getBindingDescription();

		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 tex;
	};

	struct VK_UniformBufferObj {
		glm::mat4 model;
		glm::vec3 color;
	};
}
