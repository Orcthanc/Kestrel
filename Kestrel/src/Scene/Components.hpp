#pragma once

#include <kstpch.hpp>
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"

#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"

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

	struct MeshComponent {
		MeshComponent() = default;
		MeshComponent( const std::shared_ptr<Mesh>& );
		MeshComponent( std::shared_ptr<Mesh>&& );

		MeshComponent( const MeshComponent& ) = default;
		MeshComponent( MeshComponent&& ) = default;

		MeshComponent& operator=( const MeshComponent& ) = default;
		MeshComponent& operator=( MeshComponent&& ) = default;

		operator const Mesh&();

		std::shared_ptr<Mesh> mesh;
	};

	struct MaterialComponent {
		MaterialComponent() = default;
		MaterialComponent( const Material& );
		MaterialComponent( Material&& );

		MaterialComponent( const MaterialComponent& ) = default;
		MaterialComponent( MaterialComponent&& ) = default;

		MaterialComponent& operator=( const MaterialComponent& ) = default;
		MaterialComponent& operator=( MaterialComponent&& ) = default;

		operator const Material&() const;

		Material mat;
	};
}
