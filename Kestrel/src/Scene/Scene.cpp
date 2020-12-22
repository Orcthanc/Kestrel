#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Components.hpp"

using namespace Kestrel;

void Scene::onUpdate(){

}

Entity Scene::createEntity(){
	Entity ret = { this, entt_reg.create() };

#ifndef NDEBUG
	char name[20];

	snprintf( name, 20, "Entity %u", ret.entity );
	ret.addComponent<NameComponent>( name );
#endif

	return ret;
}

Entity Scene::createEntity( const char *name ){
	Entity ret = { this, entt_reg.create() };
	ret.addComponent<NameComponent>( name );
	return ret;
}

void Scene::destroyEntity( Entity entity ){
	entt_reg.destroy( entity );
}

Entity Scene::toEntity(entt::entity e){
	return { this, e };
}
