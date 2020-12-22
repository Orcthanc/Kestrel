#pragma once

#include "Scene/Scene.hpp"

namespace Kestrel {

	struct TerrainScene: public Scene {
		public:
			TerrainScene();
			virtual ~TerrainScene() = default;

			virtual void onUpdate() override;
	};

}
