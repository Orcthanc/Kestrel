#pragma once

#include <kstpch.hpp>
#include "Renderer/Camera.hpp"

namespace Kestrel {
	struct NaiveCamera: public Camera {
		NaiveCamera( float fov, float aspect, float near_plane, float far_plane );
		virtual ~NaiveCamera() = default;

		void recalc_aspect_ratio( float aspect );
		virtual bool onSizeChange( WindowResizeEvent& e ) override;

		private:
			float fov, near_plane, far_plane;
	};
}
