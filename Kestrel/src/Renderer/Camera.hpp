#pragma once

#include <kstpch.hpp>

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Kestrel {
	struct Camera {
		public:
			virtual ~Camera() = default;

			glm::vec3 pos;
			glm::quat rot;

			void move( float x, float y, float z );
			void rotate( float x, float y, float z, float angle );
			void rotate( const glm::quat& quaternion );
			void recalc_view();

			glm::mat4 view;
			glm::mat4 proj;
	};
}
