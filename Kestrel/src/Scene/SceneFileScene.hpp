#pragma once

#include <kstpch.hpp>

#include <filesystem>

#include "Scene/Scene.hpp"
#include <glm/glm.hpp>


#include "SyntaxTree.h"

namespace Kestrel {
	enum struct SceneVariableType {
		eEntity,
		eFloat,
		eFloat2,
		eFloat3,
		eFloat4,
	};

	enum struct SceneComponentTypes {
		eTransform,
		eCamera,
	};

	struct SceneVariable {
		SceneVariableType type;
		union {
			entt::entity entity;
			float floating;
			glm::vec2 vec2;
			glm::vec3 vec3;
			glm::vec4 vec4;
		} val;
	};

	struct SceneFileScene: public Scene {
		public:
			SceneFileScene() = default;
			SceneFileScene( const std::filesystem::path& path );
			virtual ~SceneFileScene();

			void load( const std::filesystem::path& path );

			virtual void onUpdate() override;
		private:
			ast_node* functions = nullptr;
			std::unordered_map<std::string, entt::entity> entitys;
			std::unordered_map<std::string, SceneVariableType> globals;
	};
}
