#pragma once

#include <kstpch.hpp>

#include "entt.hpp"

namespace Kestrel {

	struct Entity;

	class Scene {
		public:
			Scene() = default;
			~Scene() = default;

			Scene( const Scene& ) = delete;
			Scene( Scene&& ) = default;

			Scene& operator=( const Scene& ) = delete;
			Scene& operator=( Scene&& ) = default;

			virtual void onUpdate();

			Entity createEntity();
			Entity createEntity( const char* name );
			void destroyEntity( Entity entity );

			//TODO remove
			Entity toEntity( entt::entity e );

			template<typename... T>
			auto getView(){
				return entt_reg.view<T...>();
			}

		private:
			entt::registry entt_reg;

			friend struct Entity;
	};
}
