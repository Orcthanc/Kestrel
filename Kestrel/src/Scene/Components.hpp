#pragma once

#include <kstpch.hpp>
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"

namespace Kestrel {
	struct TransformComponent {
		TransformComponent( glm::vec3 loc, glm::vec3 scale );
		TransformComponent(
				glm::vec3 loc = glm::vec3{},
				glm::quat = glm::quat{},
				glm::vec3 scale = glm::vec3{} );

		TransformComponent( const TransformComponent& ) = default;
		TransformComponent( TransformComponent&& ) = default;

		TransformComponent& operator=( const TransformComponent& ) = default;
		TransformComponent& operator=( TransformComponent&& ) = default;

		glm::vec3 loc;
		glm::quat rot;
		glm::vec3 scale;
	};

	struct NameComponent {
		NameComponent();
		NameComponent( const char* name );
		NameComponent( const std::string& name );

		NameComponent( const NameComponent& ) = default;
		NameComponent( NameComponent&& ) = default;

		NameComponent& operator=( const NameComponent& ) = default;
		NameComponent& operator=( NameComponent&& ) = default;

		operator const std::string&() const;

		std::string name;
	};
}
