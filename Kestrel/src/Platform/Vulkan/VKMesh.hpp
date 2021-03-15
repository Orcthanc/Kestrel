#pragma once

#include "Platform/Vulkan/Base.hpp"

#include "Renderer/Mesh.hpp"
#include "Platform/Vulkan/VKVertex.hpp"

#include <filesystem>
#include <map>

namespace Kestrel {
	struct VK_Mesh {
		public:
			virtual ~VK_Mesh() = default;

			void load_obj(const char *path);

			size_t vertex_offset, vertex_size;
			size_t index_offset, index_amount;
	};

	struct VK_MeshRegistry {
		public:
			static Mesh requestMesh( const std::filesystem::path& );
			static Mesh requestOrLoadMesh( const std::filesystem::path& );
			static std::shared_ptr<VK_Mesh> getMeshImpl( Mesh );

			static void initialize(
					KST_VK_DeviceSurface*,
					vk::Queue queue,
					vk::UniqueCommandPool&& cmd_pool,
					//Currently not hints but the maximum size TODO
					size_t vertex_size_hint = 100000 * sizeof( VK_Vertex ),
					size_t index_size_hint = 600000 * sizeof( uint32_t ));

			static std::map<const std::filesystem::path, Mesh> meshes;
			static std::map<Mesh, std::shared_ptr<VK_Mesh>> mesh_impls;

			static struct MeshRegData {
				bool initialized = false;
				KST_VK_Buffer vertex_buffer; 		//TODO move to mesh
				KST_VK_Buffer index_buffer; 		//TODO move to mesh
			} mesh_data;
		private:
			static KST_VK_DeviceSurface* device;

			static struct CopyInfo {
				vk::Queue queue;
				vk::UniqueCommandPool pool;
			} copy_inf;

			friend struct VK_Mesh;
			friend struct KST_VK_DeviceSurface;
	};
}
