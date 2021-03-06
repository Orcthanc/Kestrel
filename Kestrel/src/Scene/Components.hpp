#pragma once

#include <kstpch.hpp>
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"

#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"

#include "Renderer/Camera.hpp"

#include "Renderer/Terrain.hpp"

namespace Kestrel {
	struct TransformComponent {
		TransformComponent( glm::vec3 loc, glm::vec3 scale );
		TransformComponent(
				glm::vec3 loc = glm::vec3{},
				glm::quat = glm::quat{},
				glm::vec3 scale = glm::vec3{ 1.0f, 1.0f, 1.0f });

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
		MeshComponent( Mesh );

		MeshComponent( const MeshComponent& ) = default;
		MeshComponent( MeshComponent&& ) = default;

		MeshComponent& operator=( const MeshComponent& ) = default;
		MeshComponent& operator=( MeshComponent&& ) = default;

		operator const Mesh&() const;

		Mesh mesh;
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

	struct CameraComponent {
		CameraComponent() = default;
		CameraComponent( const std::shared_ptr<Camera>& );
		CameraComponent( std::shared_ptr<Camera>&& );

		CameraComponent( const CameraComponent& ) = default;
		CameraComponent( CameraComponent&& ) = default;

		CameraComponent& operator=( const CameraComponent& ) = default;
		CameraComponent& operator=( CameraComponent&& ) = default;

		operator const Camera&() const;

		std::shared_ptr<Camera> camera;
	};

	struct ColorComponent {
		ColorComponent() = default;
		ColorComponent( const glm::vec3& rgb );
		ColorComponent( glm::vec3&& rgb );

		ColorComponent( const ColorComponent& ) = default;
		ColorComponent( ColorComponent&& ) = default;

		ColorComponent& operator=( const ColorComponent& ) = default;
		ColorComponent& operator=( ColorComponent&& ) = default;

		operator const glm::vec3&() const;

		glm::vec3 color;
	};

	struct TerrainComponent {
		TerrainComponent(): terrain( null_terrain ){};
		TerrainComponent( const Terrain& terrain );
		TerrainComponent( Terrain&& terrain );

		TerrainComponent( const TerrainComponent& ) = default;
		TerrainComponent( TerrainComponent&& ) = default;

		TerrainComponent& operator=( const TerrainComponent& ) = default;
		TerrainComponent& operator=( TerrainComponent&& ) = default;

		operator const Terrain&() const;

		Terrain terrain;
	};
}
