#pragma once

#include "Scene/Scene.hpp"

#include "Renderer/Terrain.hpp"

namespace Kestrel {

	struct TerrainScene: public Scene {
		public:
			TerrainScene( const Terrain& );
			virtual ~TerrainScene() = default;

			virtual void onUpdate() override;
	};

}
