#pragma once

#include "Renderer/Mesh.hpp"
#include "Platform/Vulkan/VKVertex.hpp"

namespace Kestrel {
	struct VK_Mesh: public MeshImpl {
		~VK_Mesh() = default;

		virtual void load_obj(const char *path) override;
		virtual void unload() override;
		virtual bool loaded() override;
		virtual const BufferView<float> getVertices() override;
		virtual const BufferView<uint32_t> getIndices() override;

		std::vector<VK_Vertex3f3f> verts;
		std::vector<uint32_t> indices;
	};
}
