#pragma once

#include "Renderer/Terrain.hpp"

#include "Platform/Vulkan/Base.hpp"

namespace Kestrel {
	struct VK_Terrain_T {
		public:
			TerrainInfo info;
			uint32_t size;
			KST_VK_Buffer instance_offsets;
	};

	struct VK_TerrainRegistry {
		public:
			static Terrain createTerrain( const TerrainInfo& );

			static void init( KST_VK_DeviceSurface* );
			static void destroy();

			static std::map<Terrain, VK_Terrain_T> terrains;

		private:
			static KST_VK_DeviceSurface* device;
			static vk::UniqueCommandPool pool;
	};
}
