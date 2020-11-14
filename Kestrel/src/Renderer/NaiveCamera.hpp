#pragma once

#include <kstpch.hpp>
#include "Renderer/Camera.hpp"

namespace Kestrel {
	struct NaiveCamera: public Camera {
		NaiveCamera( float fov, float aspect, float near_plane, float far_plane );
		virtual ~NaiveCamera() = default;
	};
}
