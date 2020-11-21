#pragma once

#include "Renderer/Mesh.hpp"
#include "Platform/Vulkan/VKVertex.hpp"

#include <filesystem>
#include <map>

namespace Kestrel {
	struct VK_Mesh: public MeshImpl {
		public:
			virtual ~VK_Mesh() = default;

			static std::shared_ptr<MeshImpl> getMesh( const std::filesystem::path& );
			static std::shared_ptr<MeshImpl> create( const std::filesystem::path& );

			virtual void load_obj(const char *path) override;
			virtual const BufferView<float> getVertices() override;
			virtual const BufferView<uint32_t> getIndices() override;

			std::vector<VK_Vertex> verts;
			std::vector<uint32_t> indices;
	};


	struct VK_MeshRegistry {
		static std::shared_ptr<VK_Mesh> getMesh( const std::filesystem::path& );
		static void register_mesh( const std::filesystem::path& p, std::shared_ptr<VK_Mesh> );

		static std::map<const std::filesystem::path, std::shared_ptr<VK_Mesh>> meshes;
	};
}
