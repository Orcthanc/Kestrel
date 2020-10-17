#pragma once

#include "Renderer/Terrain.hpp"

namespace Kestrel {
	struct VK_BasicTerrain: public Terrain {
		VK_BasicTerrain();
		virtual ~VK_BasicTerrain() = default;

		virtual void draw() override;
	};
}
