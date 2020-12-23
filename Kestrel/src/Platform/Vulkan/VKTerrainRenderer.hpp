#pragma once

#include <kstpch.hpp>

#include "Renderer/Terrain.hpp"

#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"

#include "Scene/Components.hpp"

namespace Kestrel {
	struct KST_VK_CameraRenderer;

	struct KST_VK_TerrainRenderer {
		public:
			static void init();
			void drawTerrain( KST_VK_CameraRenderer* renderer, const TransformComponent& transform, const Terrain& terrain );

			static KST_VK_TerrainRenderer& get(){
				return instance;
			}
		private:
			Mesh terrain_mesh;
			Material terrain_material;
			static KST_VK_TerrainRenderer instance;
	};
}
