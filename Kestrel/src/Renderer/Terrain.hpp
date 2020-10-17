#pragma once

#include <kstpch.hpp>
#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"

namespace Kestrel {
	struct Terrain {
		virtual ~Terrain() = default;
		virtual void draw() = 0;

		Material mat;
		Mesh mesh;
	};
}
