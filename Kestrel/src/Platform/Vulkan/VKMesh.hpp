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
