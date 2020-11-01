#pragma once

#include <kstpch.hpp>
#include "Scene/Scene.hpp"

#include "entt.hpp"

namespace Kestrel {
	struct Entity {
		Entity(): scene( nullptr ), entity( entt::null ){}
		Entity( Scene* scene, entt::entity entity ): scene( scene ), entity( entity ){}

		Entity( const Entity& ) = default;
		Entity( Entity&& ) = default;

		Entity& operator=( const Entity& ) = default;
		Entity& operator=( Entity&& ) = default;

		operator bool(){
			return entity != entt::null;
		}

		operator entt::entity(){
			return entity;
		}

		template<typename T, typename... Args>
		T& addComponent( Args&&... args ){
			KST_CORE_ASSERT( !scene->entt_reg.has<T>( entity ), "Object already has component" );
			return scene->entt_reg.emplace<T>( entity, std::forward<Args>( args )... );
		}
		
		template<typename... T>
		bool hasComponent(){
			return scene->entt_reg.has<T...>( entity );
		}

		template<typename... T>
		auto getComponent(){
			KST_CORE_ASSERT( scene->entt_reg.has<T...>( entity ), "Object does not have components" );
			return scene->entt_reg.get<T...>( entity );
		}

		template<typename... T>
		void removeComponent(){
			KST_CORE_ASSERT( scene->entt_reg.has<T...>( entity ), "Object does not have component" );
			return scene->entt_reg.remove<T...>( entity );
		}

		Scene* scene;
		entt::entity entity;
	};
}
