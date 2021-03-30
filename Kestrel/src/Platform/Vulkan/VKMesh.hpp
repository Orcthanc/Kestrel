#pragma once

#include "Resource/Resource.hpp"
#include "Platform/Vulkan/Base.hpp"

#include "Renderer/Mesh.hpp"
#include "Platform/Vulkan/VKVertex.hpp"

#include <filesystem>
#include <map>

namespace Kestrel {
	struct VK_Mesh {
		public:
			virtual ~VK_Mesh() = default;

			size_t vertex_offset{}, vertex_size{};
			size_t index_offset{}, index_amount{};
	};

/*

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

*/

	struct SharedMeshResources {
		KST_VK_DeviceSurface* device;

		struct CopyInfo {
			vk::Queue queue;
			vk::UniqueCommandPool pool;
		} copy_inf;

		struct MeshRegData {
			bool initialized = false;
			KST_VK_Buffer vertex_buffer;
			KST_VK_Buffer index_buffer;
		} mesh_data;
	};

	using VK_MeshRegistry = ResourceRegistry<VK_Mesh, Mesh, SharedMeshResources, std::filesystem::path>;

	template<>template<>
	void VK_MeshRegistry::initialize(
			KST_VK_DeviceSurface*,
			vk::Queue copy_queue,
			vk::UniqueCommandPool&& copy_pool,
			size_t max_vertex_size,
			size_t max_index_size );

}
