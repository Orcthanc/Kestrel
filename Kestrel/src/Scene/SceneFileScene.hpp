#pragma once

#include <kstpch.hpp>

#include <filesystem>

#include "Scene/Scene.hpp"
#include <glm/glm.hpp>


#include "SyntaxTree.h"

namespace Kestrel {

	enum struct SceneComponentTypes {
		eTransform = 1 << 0,
		eMesh = 1 << 1,
		eMat = 1 << 2,
		eColor = 1 << 3,
		eCamera = 1 << 4,
	};

	struct SceneFileScene: public Scene {
		public:
			SceneFileScene() = default;
			SceneFileScene( const std::filesystem::path& path );
			virtual ~SceneFileScene();

			void load( const std::filesystem::path& path );

			virtual void onUpdate() override;
		private:
			void callFunctions();
			ast_node* functions = nullptr;
			std::unordered_map<std::string, entt::entity> entitys;
			std::unordered_map<entt::entity, SceneComponentTypes> components;
	};
}
